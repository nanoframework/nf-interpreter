//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#pragma once

#include <Target_Network.h>

constexpr int NetworkTls_Error = -1;
constexpr int NetworkTls_WantRead = -2;
constexpr int NetworkTls_WantWrite = -3;

int NetworkTls_CreateContext(int protocols, const uint8_t *ca, size_t length, bool verifyPeer);
int NetworkTls_CreateServerContext(
    int protocols,
    const uint8_t *certificate,
    size_t certificateLength,
    const uint8_t *privateKey,
    size_t privateKeyLength,
    const char *password);
bool NetworkTls_DecodePrivateKey(const uint8_t *data, size_t length, const char *password);
bool NetworkTls_FreeContext(int context);
int NetworkTls_Connect(int context, int socket, const char *host);
int NetworkTls_Accept(int context, int socket);
int NetworkTls_Read(int socket, uint8_t *buffer, int length);
int NetworkTls_Write(int socket, const uint8_t *buffer, int length);
int NetworkTls_Available(int socket);
int NetworkTls_LastError();
void NetworkTls_Close(int socket);
void NetworkTls_Reset();
bool NetworkTls_SetCaBundle(const uint8_t *data, size_t length);
bool NetworkTls_ParseCertificate(const uint8_t *data, size_t length, X509CertData &certificate);

bool HostNetwork_ArmSocket(int socket, bool write);
