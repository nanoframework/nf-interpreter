//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef RMT_MULTI_STAGE_ENCODER_H  
#define RMT_MULTI_STAGE_ENCODER_H 

enum RmtEncoderType
{
    RmtEncoderType_None = 0,
    RmtEncoderType_Copy,
    RmtEncoderType_Byte
};

typedef struct
{
    rmt_encoder_t *base;
    RmtEncoderType encType;

    // encoder handle
    rmt_encoder_t *encoder;

    int loopCount;
    int currentLoop;

    bool dataRequired;

    // Type depends on encode type
    void *data;

    // data length
    uint dataLen;

} rmt_multi_item_t;

typedef struct
{
    rmt_encoder_t base;

    int numberSteps;
    int step;

    // Ptr to array of rmt_multi_item_t
    rmt_multi_item_t *encoders;

} rmt_multi_stage_encoder_t;

esp_err_t rmt_new_multi_stage_encoder(rmt_encoder_handle_t *ret_encoder, int numberEncoders);
esp_err_t rmt_add_byte_encoder(rmt_encoder_t *encoder, int loopCount, rmt_bytes_encoder_config_t &config);
esp_err_t rmt_add_copy_encoder(
    rmt_encoder_t *encoder,
    int loopCount,
    rmt_copy_encoder_config_t &config,
    rmt_symbol_word_t *symbols,
    uint dataLen);

#endif // RMT_MULTI_STAGE_ENCODER_H
