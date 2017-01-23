//
// Copyright (c) 2017 The nano Framework project contributors
// See LICENSE file in the project root for full license information.
//

#include <stdint.h>
#include "WireProtocol.h"

uint8_t* marker;
uint16_t lastOutboundMessage;
uint32_t lastPacketSequence;

WP_Message inboundMessage;
