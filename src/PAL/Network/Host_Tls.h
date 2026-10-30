//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#pragma once

#include <Host_Network.h>

constexpr int HostTls_Error = -1;
constexpr int HostTls_WantRead = -2;
constexpr int HostTls_WantWrite = -3;

int HostTls_CreateContext(int protocols, const uint8_t *ca, size_t length, bool verifyPeer);
int HostTls_CreateServerContext(
    int protocols,
    const uint8_t *certificate,
    size_t certificateLength,
    const uint8_t *privateKey,
    size_t privateKeyLength,
    const char *password);
bool HostTls_DecodePrivateKey(const uint8_t *data, size_t length, const char *password);
bool HostTls_FreeContext(int context);
int HostTls_Connect(int context, int socket, const char *host);
int HostTls_Accept(int context, int socket);
int HostTls_Read(int socket, uint8_t *buffer, int length);
int HostTls_Write(int socket, const uint8_t *buffer, int length);
int HostTls_Available(int socket);
int HostTls_LastError();
void HostTls_Close(int socket);
void HostTls_Reset();
bool HostTls_SetCaBundle(const uint8_t *data, size_t length);
bool HostTls_ParseCertificate(const uint8_t *data, size_t length, X509CertData &certificate);

bool HostNetwork_ArmSocket(int socket, bool write);
