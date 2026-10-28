//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#pragma once

#include <Target_Network.h>

constexpr int LinuxTls_Error = -1;
constexpr int LinuxTls_WantRead = -2;
constexpr int LinuxTls_WantWrite = -3;

int LinuxTls_CreateContext(int protocols, const uint8_t *ca, size_t length, bool verifyPeer);
int LinuxTls_CreateServerContext(
    int protocols,
    const uint8_t *certificate,
    size_t certificateLength,
    const uint8_t *privateKey,
    size_t privateKeyLength,
    const char *password);
bool LinuxTls_DecodePrivateKey(const uint8_t *data, size_t length, const char *password);
bool LinuxTls_FreeContext(int context);
int LinuxTls_Connect(int context, int socket, const char *host);
int LinuxTls_Accept(int context, int socket);
int LinuxTls_Read(int socket, uint8_t *buffer, int length);
int LinuxTls_Write(int socket, const uint8_t *buffer, int length);
int LinuxTls_Available(int socket);
int LinuxTls_LastError();
void LinuxTls_Close(int socket);
void LinuxTls_Reset();
bool LinuxTls_SetCaBundle(const uint8_t *data, size_t length);
bool LinuxTls_ParseCertificate(const uint8_t *data, size_t length, X509CertData &certificate);

bool LinuxNetwork_ArmSocket(int socket, bool write);
