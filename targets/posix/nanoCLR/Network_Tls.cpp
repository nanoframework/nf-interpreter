//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <Target_Tls.h>

#include <arpa/inet.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/error.h>
#include <mbedtls/pk.h>
#include <mbedtls/platform_util.h>
#include <mbedtls/ssl.h>
#include <mbedtls/x509_crt.h>
#include <psa/crypto.h>
#include <psa/crypto_extra.h>

#include <algorithm>
#include <cerrno>
#include <cctype>
#include <climits>
#include <cstdlib>
#include <fstream>
#include <map>
#include <memory>
#include <mutex>
#include <string_view>

namespace
{
constexpr size_t c_maxCertificateBytes = 1024 * 1024;

struct Certificates
{
    mbedtls_x509_crt chain;

    Certificates()
    {
        mbedtls_x509_crt_init(&chain);
    }

    ~Certificates()
    {
        mbedtls_x509_crt_free(&chain);
    }
};

struct Context
{
    mbedtls_ssl_config config;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context random;
    mbedtls_pk_context privateKey;
    std::shared_ptr<Certificates> certificates;
    std::shared_ptr<Certificates> identity;
    bool server = false;
    bool verifyPeer = true;

    Context()
    {
        mbedtls_ssl_config_init(&config);
        mbedtls_entropy_init(&entropy);
        mbedtls_ctr_drbg_init(&random);
        mbedtls_pk_init(&privateKey);
    }

    ~Context()
    {
        mbedtls_ssl_config_free(&config);
        mbedtls_pk_free(&privateKey);
        mbedtls_ctr_drbg_free(&random);
        mbedtls_entropy_free(&entropy);
    }
};

struct Connection
{
    int context;
    int socket;
    std::string host;
    std::shared_ptr<Context> settings;
    mbedtls_ssl_context session;
    std::vector<uint8_t> pendingWrite;
    int transportError = 0;
    bool ready = false;
    bool closed = false;
    bool failed = false;

    Connection(int id, int handle, const char *name, std::shared_ptr<Context> config)
        : context(id), socket(handle), host(name), settings(std::move(config))
    {
        mbedtls_ssl_init(&session);
    }

    ~Connection()
    {
        mbedtls_ssl_free(&session);
    }
};

std::mutex s_tlsMutex;
std::map<int, std::shared_ptr<Context>> s_contexts;
std::map<int, std::unique_ptr<Connection>> s_connections;
std::shared_ptr<Certificates> s_caBundle;
int64_t s_nextContext = 1;
thread_local int s_tlsError = 0;

int Failure(const char *operation, int error)
{
    s_tlsError = error;
    char message[256] = {};
    if (error < 0)
    {
        mbedtls_strerror(error, message, sizeof(message));
    }
    fprintf(stderr, "nanoCLR TLS %s failed (%d)%s%s\n", operation, error, message[0] ? ": " : "", message);
    return LinuxTls_Error;
}

bool InitializeRandom(Context &context)
{
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS)
    {
        fprintf(stderr, "nanoCLR TLS PSA initialization failed (%ld)\n", static_cast<long>(status));
        Failure("cryptographic initialization", MBEDTLS_ERR_SSL_INTERNAL_ERROR);
        return false;
    }
    constexpr unsigned char personalization[] = "nanoFramework POSIX TLS";
    int result = mbedtls_ctr_drbg_seed(
        &context.random,
        mbedtls_entropy_func,
        &context.entropy,
        personalization,
        sizeof(personalization) - 1);
    if (result != 0)
    {
        Failure("host entropy", result);
        return false;
    }
    return true;
}

bool ParsePrivateKey(Context &context, const uint8_t *data, size_t length, const char *password)
{
    if (data == nullptr || length == 0 || length > c_maxCertificateBytes)
    {
        Failure("private key input", SOCK_EINVAL);
        return false;
    }
    std::vector<uint8_t> pem;
    if (data[0] != 0x30 && data[length - 1] != 0)
    {
        pem.assign(data, data + length);
        pem.push_back(0);
        data = pem.data();
        length = pem.size();
    }
    int result = mbedtls_pk_parse_key(
        &context.privateKey,
        data,
        length,
        reinterpret_cast<const unsigned char *>(password),
        password == nullptr ? 0 : strlen(password),
        mbedtls_ctr_drbg_random,
        &context.random);
    if (!pem.empty())
    {
        mbedtls_platform_zeroize(pem.data(), pem.size());
    }
    if (result != 0)
    {
        Failure("private key parsing", result);
        return false;
    }
    return true;
}

std::shared_ptr<Context> MakeContext(int protocols, bool server)
{
    if (s_contexts.size() >= 16 || s_nextContext > INT_MAX)
    {
        Failure("context capacity", SOCK_EMFILE);
        return nullptr;
    }
    if (protocols != 0 && protocols != 0xC00 && protocols != 0x3000 && protocols != 0x3C00)
    {
        Failure("unsupported TLS protocol (TLS 1.2/1.3 only)", SOCK_EOPNOTSUPP);
        return nullptr;
    }
    auto context = std::make_shared<Context>();
    context->server = server;
    if (!InitializeRandom(*context))
    {
        return nullptr;
    }
    int result = mbedtls_ssl_config_defaults(
        &context->config,
        server ? MBEDTLS_SSL_IS_SERVER : MBEDTLS_SSL_IS_CLIENT,
        MBEDTLS_SSL_TRANSPORT_STREAM,
        MBEDTLS_SSL_PRESET_DEFAULT);
    if (result != 0)
    {
        Failure("context creation", result);
        return nullptr;
    }
    mbedtls_ssl_conf_rng(&context->config, mbedtls_ctr_drbg_random, &context->random);
    mbedtls_ssl_conf_authmode(&context->config, server ? MBEDTLS_SSL_VERIFY_NONE : MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_min_tls_version(
        &context->config,
        protocols == 0x3000 ? MBEDTLS_SSL_VERSION_TLS1_3 : MBEDTLS_SSL_VERSION_TLS1_2);
    mbedtls_ssl_conf_max_tls_version(
        &context->config,
        protocols == 0xC00 ? MBEDTLS_SSL_VERSION_TLS1_2 : MBEDTLS_SSL_VERSION_TLS1_3);
    return context;
}

std::shared_ptr<Certificates> ParseCertificates(const uint8_t *data, size_t length, bool annotatedFile = false)
{
    if (data == nullptr || length == 0 || length > c_maxCertificateBytes)
    {
        Failure("certificate input", SOCK_EINVAL);
        return nullptr;
    }
    auto certificates = std::make_shared<Certificates>();
    int result;
    if (data[0] == 0x30)
    {
        result = mbedtls_x509_crt_parse_der(&certificates->chain, data, length);
        if (result == 0 && certificates->chain.raw.len != length)
        {
            result = MBEDTLS_ERR_X509_INVALID_FORMAT;
        }
    }
    else if (annotatedFile)
    {
        std::vector<uint8_t> pem(data, data + length);
        pem.push_back(0);
        result = mbedtls_x509_crt_parse(&certificates->chain, pem.data(), pem.size());
    }
    else
    {
        std::string_view remaining(reinterpret_cast<const char *>(data), length);
        constexpr std::string_view begin = "-----BEGIN CERTIFICATE-----";
        constexpr std::string_view end = "-----END CERTIFICATE-----";
        result = 0;
        while (!remaining.empty())
        {
            if (std::isspace(static_cast<unsigned char>(remaining.front())) || remaining.front() == '\0')
            {
                remaining.remove_prefix(1);
                continue;
            }
            size_t finish = remaining.find(end);
            if (!remaining.starts_with(begin) || finish == std::string_view::npos)
            {
                result = MBEDTLS_ERR_X509_INVALID_FORMAT;
                break;
            }
            size_t size = finish + end.size();
            std::vector<uint8_t> pem(remaining.begin(), remaining.begin() + size);
            pem.push_back(0);
            result = mbedtls_x509_crt_parse(&certificates->chain, pem.data(), pem.size());
            if (result != 0)
            {
                break;
            }
            remaining.remove_prefix(size);
        }
    }
    if (result != 0 || certificates->chain.raw.len == 0)
    {
        Failure("certificate parsing", result < 0 ? result : MBEDTLS_ERR_X509_INVALID_FORMAT);
        return nullptr;
    }
    return certificates;
}

std::shared_ptr<Certificates> ReadCaFile()
{
    const char *configured = getenv("NANOCLR_CA_BUNDLE");
    if (configured == nullptr)
    {
        Failure("no trusted CA configured; supply a managed CA, NANOCLR_CA_BUNDLE or --ca-bundle", SOCK_EINVAL);
        return nullptr;
    }
    std::string path(configured);
    if (path.empty())
    {
        Failure("empty NANOCLR_CA_BUNDLE", SOCK_EINVAL);
        return nullptr;
    }
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        fprintf(stderr, "nanoCLR TLS cannot open CA bundle '%s': %s\n", path.c_str(), strerror(errno));
        Failure("CA bundle file", SOCK_EINVAL);
        return nullptr;
    }
    auto length = file.tellg();
    if (length <= 0 || length > static_cast<std::streamoff>(c_maxCertificateBytes))
    {
        fprintf(stderr, "nanoCLR TLS empty, unreadable or oversized CA bundle '%s'\n", path.c_str());
        Failure("CA bundle file size", SOCK_EINVAL);
        return nullptr;
    }
    std::vector<uint8_t> data(static_cast<size_t>(length));
    file.seekg(0);
    if (!file.read(reinterpret_cast<char *>(data.data()), data.size()))
    {
        fprintf(stderr, "nanoCLR TLS cannot read CA bundle '%s'\n", path.c_str());
        Failure("CA bundle file read", SOCK_EINVAL);
        return nullptr;
    }
    auto certificates = ParseCertificates(data.data(), data.size(), true);
    if (!certificates)
    {
        fprintf(stderr, "nanoCLR TLS invalid CA bundle '%s'\n", path.c_str());
    }
    return certificates;
}

int Receive(void *opaque, unsigned char *buffer, size_t length)
{
    auto &connection = *static_cast<Connection *>(opaque);
    int result = SOCK_recv(connection.socket, reinterpret_cast<char *>(buffer), std::min(length, size_t(INT_MAX)), 0);
    if (result >= 0)
    {
        return result;
    }
    int error = SOCK_getlasterror();
    if (error == SOCK_EWOULDBLOCK)
    {
        return MBEDTLS_ERR_SSL_WANT_READ;
    }
    connection.transportError = error;
    return MBEDTLS_ERR_SSL_INTERNAL_ERROR;
}

int Send(void *opaque, const unsigned char *buffer, size_t length)
{
    auto &connection = *static_cast<Connection *>(opaque);
    int result =
        SOCK_send(connection.socket, reinterpret_cast<const char *>(buffer), std::min(length, size_t(INT_MAX)), 0);
    if (result >= 0)
    {
        return result;
    }
    int error = SOCK_getlasterror();
    if (error == SOCK_EWOULDBLOCK)
    {
        return MBEDTLS_ERR_SSL_WANT_WRITE;
    }
    connection.transportError = error;
    return MBEDTLS_ERR_SSL_INTERNAL_ERROR;
}

int VerifyIp(void *opaque, mbedtls_x509_crt *certificate, int depth, uint32_t *flags)
{
    if (depth == 0)
    {
        auto &connection = *static_cast<Connection *>(opaque);
        uint32_t identityFlags = 0;
        int result = mbedtls_x509_crt_verify(
            certificate,
            &connection.settings->certificates->chain,
            nullptr,
            connection.host.c_str(),
            &identityFlags,
            nullptr,
            nullptr);
        *flags |= identityFlags;
        if (result != 0 && result != MBEDTLS_ERR_X509_CERT_VERIFY_FAILED)
        {
            return result;
        }
    }
    return 0;
}

int TranslateResult(Connection &connection, int result)
{
    if (result == MBEDTLS_ERR_SSL_WANT_READ || result == MBEDTLS_ERR_SSL_WANT_WRITE)
    {
        if (LinuxNetwork_ArmSocket(connection.socket, result == MBEDTLS_ERR_SSL_WANT_WRITE))
        {
            return result == MBEDTLS_ERR_SSL_WANT_READ ? LinuxTls_WantRead : LinuxTls_WantWrite;
        }
        connection.failed = true;
        return Failure("closed socket", SOCK_ENOTSOCK);
    }
    connection.failed = true;
    if (connection.transportError != 0)
    {
        return Failure("socket I/O", connection.transportError);
    }
    uint32_t flags = mbedtls_ssl_get_verify_result(&connection.session);
    if (!connection.settings->server && connection.settings->verifyPeer && flags != 0 && flags != UINT32_MAX)
    {
        char message[512] = {};
        mbedtls_x509_crt_verify_info(message, sizeof(message), "", flags);
        fprintf(stderr, "nanoCLR TLS certificate verification: %s\n", message);
    }
    return Failure("I/O or handshake", result);
}

void ReadDate(const mbedtls_x509_time &value, DATE_TIME_INFO &result)
{
    result = {};
    result.year = value.year;
    result.month = value.mon;
    result.day = value.day;
    result.hour = value.hour;
    result.minute = value.min;
    result.second = value.sec;
}
} // namespace

int LinuxTls_CreateContext(int protocols, const uint8_t *ca, size_t length, bool verifyPeer)
{
    std::lock_guard<std::mutex> lock(s_tlsMutex);
    auto context = MakeContext(protocols, false);
    if (!context)
    {
        return LinuxTls_Error;
    }
    context->verifyPeer = verifyPeer;
    if (verifyPeer)
    {
        context->certificates = ca != nullptr || length != 0 ? ParseCertificates(ca, length) : s_caBundle;
        if (!context->certificates && ca == nullptr && length == 0 && !s_caBundle)
        {
            context->certificates = ReadCaFile();
        }
        if (!context->certificates)
        {
            return LinuxTls_Error;
        }
        mbedtls_ssl_conf_ca_chain(&context->config, &context->certificates->chain, nullptr);
    }
    else
    {
        mbedtls_ssl_conf_authmode(&context->config, MBEDTLS_SSL_VERIFY_NONE);
    }
    int id = static_cast<int>(s_nextContext++);
    s_contexts.emplace(id, std::move(context));
    return id;
}

int LinuxTls_CreateServerContext(
    int protocols,
    const uint8_t *certificate,
    size_t certificateLength,
    const uint8_t *privateKey,
    size_t privateKeyLength,
    const char *password)
{
    std::lock_guard<std::mutex> lock(s_tlsMutex);
    auto context = MakeContext(protocols, true);
    if (!context)
    {
        return LinuxTls_Error;
    }
    context->identity = ParseCertificates(certificate, certificateLength);
    if (!context->identity || !ParsePrivateKey(*context, privateKey, privateKeyLength, password))
    {
        return LinuxTls_Error;
    }
    int result = mbedtls_pk_check_pair(
        &context->identity->chain.pk,
        &context->privateKey,
        mbedtls_ctr_drbg_random,
        &context->random);
    if (result != 0)
    {
        return Failure("server certificate/private key mismatch", result);
    }
    result = mbedtls_ssl_conf_own_cert(&context->config, &context->identity->chain, &context->privateKey);
    if (result != 0)
    {
        return Failure("server identity configuration", result);
    }
    int id = static_cast<int>(s_nextContext++);
    s_contexts.emplace(id, std::move(context));
    return id;
}

bool LinuxTls_DecodePrivateKey(const uint8_t *data, size_t length, const char *password)
{
    std::lock_guard<std::mutex> lock(s_tlsMutex);
    Context context;
    return InitializeRandom(context) && ParsePrivateKey(context, data, length, password);
}

bool LinuxTls_FreeContext(int context)
{
    std::lock_guard<std::mutex> lock(s_tlsMutex);
    for (auto it = s_connections.begin(); it != s_connections.end();)
    {
        it = it->second->context == context ? s_connections.erase(it) : std::next(it);
    }
    return s_contexts.erase(context) != 0;
}

namespace
{
int Handshake(int context, int socket, const char *host, bool server)
{
    std::lock_guard<std::mutex> lock(s_tlsMutex);
    auto found = s_contexts.find(context);
    if (found == s_contexts.end() || found->second->server != server ||
        (!server && (host == nullptr || host[0] == '\0')))
    {
        return Failure("invalid context or target host", SOCK_EINVAL);
    }
    auto existing = s_connections.find(socket);
    if (existing == s_connections.end())
    {
        auto connection = std::make_unique<Connection>(context, socket, server ? "" : host, found->second);
        int result = mbedtls_ssl_setup(&connection->session, &found->second->config);
        if (result == 0 && !server)
        {
            in_addr address;
            bool numeric = inet_pton(AF_INET, host, &address) == 1;
            result = mbedtls_ssl_set_hostname(&connection->session, numeric ? nullptr : host);
            if (numeric && connection->settings->verifyPeer)
            {
                // Validate IP SANs separately so an IP literal is never sent as DNS SNI.
                mbedtls_ssl_set_verify(&connection->session, VerifyIp, connection.get());
            }
        }
        if (result != 0)
        {
            return Failure("session creation/hostname", result);
        }
        mbedtls_ssl_set_bio(&connection->session, connection.get(), Send, Receive, nullptr);
        existing = s_connections.emplace(socket, std::move(connection)).first;
    }
    auto &connection = *existing->second;
    if (connection.context != context || (!server && connection.host != host) || connection.failed)
    {
        return Failure("invalid session reuse", SOCK_EINVAL);
    }
    if (connection.ready)
    {
        return 0;
    }
    int result = mbedtls_ssl_handshake(&connection.session);
    if (result == 0)
    {
        connection.ready = true;
        return 0;
    }
    return TranslateResult(connection, result);
}
} // namespace

int LinuxTls_Connect(int context, int socket, const char *host)
{
    return Handshake(context, socket, host, false);
}

int LinuxTls_Accept(int context, int socket)
{
    return Handshake(context, socket, nullptr, true);
}

int LinuxTls_Read(int socket, uint8_t *buffer, int length)
{
    std::lock_guard<std::mutex> lock(s_tlsMutex);
    auto found = s_connections.find(socket);
    if (found == s_connections.end() || !found->second->ready || found->second->failed || length < 0 ||
        (length > 0 && buffer == nullptr))
    {
        return Failure("invalid read", SOCK_EINVAL);
    }
    auto &connection = *found->second;
    if (length == 0 || connection.closed)
    {
        return 0;
    }
    int result = mbedtls_ssl_read(&connection.session, buffer, length);
    if (result > 0)
    {
        return result;
    }
    if (result == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
    {
        connection.closed = true;
        return 0;
    }
    return TranslateResult(connection, result == 0 ? MBEDTLS_ERR_SSL_CONN_EOF : result);
}

int LinuxTls_Write(int socket, const uint8_t *buffer, int length)
{
    std::lock_guard<std::mutex> lock(s_tlsMutex);
    auto found = s_connections.find(socket);
    if (found == s_connections.end() || !found->second->ready || found->second->failed || found->second->closed ||
        length < 0 || (length > 0 && buffer == nullptr))
    {
        return Failure("invalid write", SOCK_EINVAL);
    }
    if (length == 0)
    {
        return 0;
    }
    auto &connection = *found->second;
    int limit = mbedtls_ssl_get_max_out_record_payload(&connection.session);
    if (limit <= 0)
    {
        return Failure("TLS record capacity", limit < 0 ? limit : MBEDTLS_ERR_SSL_INTERNAL_ERROR);
    }
    size_t count = std::min(length, limit);
    if (connection.pendingWrite.empty())
    {
        // mbedTLS retries require stable arguments, even if the managed buffer moves during GC.
        connection.pendingWrite.assign(buffer, buffer + count);
    }
    else if (connection.pendingWrite.size() != count || memcmp(buffer, connection.pendingWrite.data(), count) != 0)
    {
        return Failure("changed pending write", SOCK_EINVAL);
    }
    int result = mbedtls_ssl_write(&connection.session, connection.pendingWrite.data(), count);
    if (result > 0)
    {
        connection.pendingWrite.clear();
        return result;
    }
    return TranslateResult(connection, result == 0 ? MBEDTLS_ERR_SSL_INTERNAL_ERROR : result);
}

int LinuxTls_Available(int socket)
{
    std::lock_guard<std::mutex> lock(s_tlsMutex);
    auto found = s_connections.find(socket);
    if (found == s_connections.end() || !found->second->ready || found->second->failed)
    {
        return Failure("invalid availability query", SOCK_ENOTCONN);
    }
    return static_cast<int>(mbedtls_ssl_get_bytes_avail(&found->second->session));
}

int LinuxTls_LastError()
{
    return s_tlsError;
}

void LinuxTls_Close(int socket)
{
    std::lock_guard<std::mutex> lock(s_tlsMutex);
    auto found = s_connections.find(socket);
    if (found != s_connections.end())
    {
        // Best effort, nonblocking close_notify; never wait for the peer on disposal.
        if (found->second->ready && !found->second->failed)
        {
            mbedtls_ssl_close_notify(&found->second->session);
        }
        s_connections.erase(found);
    }
}

void LinuxTls_Reset()
{
    std::lock_guard<std::mutex> lock(s_tlsMutex);
    s_connections.clear();
    s_contexts.clear();
    s_caBundle.reset();
    mbedtls_psa_crypto_free();
}

bool LinuxTls_SetCaBundle(const uint8_t *data, size_t length)
{
    auto certificates = ParseCertificates(data, length);
    if (!certificates)
    {
        return false;
    }
    std::lock_guard<std::mutex> lock(s_tlsMutex);
    s_caBundle = std::move(certificates);
    return true;
}

bool LinuxTls_ParseCertificate(const uint8_t *data, size_t length, X509CertData &certificate)
{
    auto certificates = ParseCertificates(data, length);
    if (!certificates)
    {
        return false;
    }
    auto &first = certificates->chain;
    certificate = {};
    int issuer = mbedtls_x509_dn_gets(certificate.Issuer, sizeof(certificate.Issuer), &first.issuer);
    int subject = mbedtls_x509_dn_gets(certificate.Subject, sizeof(certificate.Subject), &first.subject);
    if (issuer < 0 || subject < 0)
    {
        Failure("certificate metadata", issuer < 0 ? issuer : subject);
        return false;
    }
    ReadDate(first.valid_from, certificate.EffectiveDate);
    ReadDate(first.valid_to, certificate.ExpirationDate);
    return true;
}
