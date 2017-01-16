# Wire Protocol

## Table of contents ##

- [Wire Protocol Packet](#WireProtocolPacket)
- [Data channels](#Datachannels)
- [Receiving and transmiting data](#Receivingandtransmitingdata)
- [Receiver workflow](#Receiverworkflow)
- [Commands](#Commands)
- [How to add support for a new command](#Howtoaddsupportforanewcommand)

**About this document**

This document describes the Wire Procotol used by **nanoFramework** for debug and booter stage.
The protocol follows the implementation of the .NET Micro Framework Wire Protocol. The intention is to review it later in order to improve and simplify it.

# Wire Protocol Message

The message basic structure is comprised by:
- Signature wich is basicaly a marker to detect the start of a new message packet. Has a fixed length.
- Header with several fields to cary packet sequence, flags, commands, CRC, etc. Has a fixed length.
  - CRC32 of header (for verification calculation this CRC32 field has to be zeroed).
  - CRC32 of payload, when it exists (for verification calculation this CRC32 field has to be zeroed).
  - Command code.
  - Sequence number of the message.
  - Sequence reply. Carries the sequence number of the message that the message is a reply to.
  - Flags.
  - Size of the payload.
- Payload for carrying data. Optional and its size is variable.

You can check the details on [WireProtocol.h](WireProtocol.h)

# Data channels

Currently **nanoFramework** Wire Protocol supports only serial channels. The plan is to add support for USB (using CDC class device) and TCP.
To ease the port to new HAL/platforms the code is architectured so that only minimal changes are required to add support for new implementations. 

# Receiving and transmiting data

The code is architectured to receive and transmit data over a serial stream.
Preferably (and to use the reference implementation provided without much changes) the interface/API of the serial stream should:
- Allow checking if there is data available for reading.
- Allow reading sequencialy (FIFO fashion) the input stream for a definite number of bytes. Having a timeout for the read operation is ideal to prevent bad/incomplete read operations.
- Allow writing to the transmit stream a definite number of bytes. Ideally in a non-blocking fashion to prevent bad/incomplete write operations.

# Receiver workflow

Follows a high-level drescription on how the Wire Protocol component works.

- RTOS thread - ```ReceiverThread(...)``` in [WireProtocol_Receiver.c](WireProtocol_Receiver.c) - that loops continuously checking for available data in the receiving channel.
- On available data the reception of the message is initialized (WP_Message_Initialize) and prepared (WP_Message_PrepareReception) so the reception can actually occurr and be processed by calling WP_Message_Process.
- During the reception states the input stream is read (```ReceiveBytes(...)``` in [WireProtocol_Receiver.c](WireProtocol_Receiver.c)) so the message header is received and it's integrity checked. Follows the reception and the integrity check of the payload, if there is any.
- After a succesfull reception of the header (and payload, if any) the _Process_ state machine reaches the ```ReceiveState_CompletePayload``` state and calls the ```ProcessPayload(...)``` function.
- Inside ```ProcessPayload(...)``` the lookup table for the commands that are implemented is searched and, if the command is found, the respective handler is called. According to the command its processing can require extra processing or gathering data. Invariably the handler execution end with a call to ```ReplyToCommand(...)``` where the reply is sent back to the host device.
- When executing ```ReplyToCommand(...)``` the output stream is written (```TransmitMessage(...)``` in [WireProtocol_Receiver.c](WireProtocol_Receiver.c)) with the reply message.

# Commands

Processing a command is carried in a handler function. 
The collection of the commands that are implemented resides in ```c_Lookup_Request```. This lookup structure is basially an array with the command code a pointer to its handler.

# How to add support for a new command

New commands are to be added inside [WireProtocol_Commands.c](WireProtocol_Commands.c).

Add the function declaration and any required structure and/or type definition in [WireProtocol_Commands.h](WireProtocol_Commands.h).
The actual code for the command handler function (and any requried helper functions or extra processing) goes into [WireProtocol_Commands.c](WireProtocol_Commands.c).

To add the command to the collection of the supported commands uncomment or add the respective line in the ```c_Lookup_Request``` variable in [WireProtocol_Commands.c](WireProtocol_Commands.c).
Because this declaration uses a macro to add the declaration of a command, make sure the existing naming pattern is _**strictly**_ followed.

To ease code portability from .NET Micro Framework code base and maintain an understandible implementation the namings have been maintained or minimally adapted from the original C++ code.
Try to follow this as much as possible when implementing new commands or porting the original C++ code to C.
