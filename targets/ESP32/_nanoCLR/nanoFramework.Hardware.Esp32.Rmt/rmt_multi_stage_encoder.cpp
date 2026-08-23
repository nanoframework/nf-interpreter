//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "esp_check.h"
#include "nanoFramework_hardware_esp32_rmt_native.h"

static const char *TAG = "multi_encoder";

typedef struct
{
} multi_stage_encoder_config_t;

IRAM_ATTR static size_t rmt_encode_multi_stage(
    rmt_encoder_t *encoder,
    rmt_channel_handle_t channel,
    const void *primary_data,
    size_t data_size,
    rmt_encode_state_t *ret_state)
{
    rmt_multi_stage_encoder_t *multi_encoder = __containerof(encoder, rmt_multi_stage_encoder_t, base);

    rmt_encode_state_t session_state = RMT_ENCODING_RESET;
    int state = RMT_ENCODING_RESET;
    size_t encoded_symbols = 0;

    // For each encoder step in multi stage encoder
    while (multi_encoder->step < multi_encoder->numberSteps)
    {
        // Get current encoder for step
        rmt_multi_item_t *mstep = multi_encoder->encoders + multi_encoder->step;
        rmt_encoder_t *stepEncoder = mstep->encoder;

        // Anything to do on this step ?
        if (mstep->dataLen == 0 || mstep->data == NULL)
        {
            //  Nothing to do, no data, next step
            multi_encoder->step++;
            continue;
        }

        //  Call encoder
        encoded_symbols +=
            stepEncoder->encode(stepEncoder, channel, (void *)mstep->data, mstep->dataLen, &session_state);

        // Has this encoder completed ?
        if (session_state & RMT_ENCODING_COMPLETE)
        {
            // Any more loops of current step ?
            mstep->currentLoop++;
            if (mstep->currentLoop < mstep->loopCount)
            {
                // yes, loop and send same data again
                rmt_encoder_reset(stepEncoder);
            }
            else
            {
                // This encoding step complete. go to next step
                multi_encoder->step++;
            }
        }

        // Did step have a memory full condition
        if (session_state & RMT_ENCODING_MEM_FULL)
        {
            // yes, then we need to yield - exit
            state = RMT_ENCODING_MEM_FULL;
            break;
        }
    } // while step

    // All steps complete, end
    if (multi_encoder->step == multi_encoder->numberSteps)
    {
        state = RMT_ENCODING_COMPLETE;
    }

    *ret_state = (rmt_encode_state_t)state;

    return encoded_symbols;
}

static esp_err_t rmt_del_multi_stage_encoder(rmt_encoder_t *encoder)
{
    rmt_multi_stage_encoder_t *multi_encoder = __containerof(encoder, rmt_multi_stage_encoder_t, base);

    for (int index = 0; index < multi_encoder->numberSteps; index++)
    {
        // note don't need to delete copy symbols as ptr to managed heap
        if ((multi_encoder->encoders + index)->encType != RmtEncoderType_None)
        {
            rmt_del_encoder((multi_encoder->encoders + index)->encoder);
            (multi_encoder->encoders + index)->encType = RmtEncoderType_None;
        }
    }

    if (multi_encoder->encoders)
    {
        platform_free(multi_encoder->encoders);
    }

    platform_free(multi_encoder);

    return ESP_OK;
}

static esp_err_t rmt_multi_stage_encoder_reset(rmt_encoder_t *encoder)
{
    rmt_multi_stage_encoder_t *multi_encoder = __containerof(encoder, rmt_multi_stage_encoder_t, base);

    for (int index = 0; index < multi_encoder->numberSteps; index++)
    {
        rmt_multi_item_t *item = (multi_encoder->encoders + index);
        if (item->encType != RmtEncoderType_None)  
        {
            rmt_encoder_reset(item->encoder);
        }
    
        item->currentLoop = 0;
    }

    multi_encoder->step = 0;
    return ESP_OK;
}

esp_err_t rmt_new_multi_stage_encoder(rmt_encoder_handle_t *ret_encoder, int numberEncoders)
{
    esp_err_t ret = ESP_OK;
    rmt_multi_stage_encoder_t *multi_encoder = NULL;

    multi_encoder = (rmt_multi_stage_encoder_t *)rmt_alloc_encoder_mem(sizeof(rmt_multi_stage_encoder_t));
    ESP_GOTO_ON_FALSE(multi_encoder, ESP_ERR_NO_MEM, err, TAG, "no mem for multi stage encoder");

    multi_encoder->base.encode = rmt_encode_multi_stage;
    multi_encoder->base.del = rmt_del_multi_stage_encoder;
    multi_encoder->base.reset = rmt_multi_stage_encoder_reset;
    multi_encoder->numberSteps = numberEncoders;
    multi_encoder->step = 0;
    multi_encoder->encoders = NULL;

    if (numberEncoders > 0)
    {
        multi_encoder->encoders = (rmt_multi_item_t *)rmt_alloc_encoder_mem(sizeof(rmt_multi_item_t) * numberEncoders);
        ESP_GOTO_ON_FALSE(multi_encoder->encoders, ESP_ERR_NO_MEM, err, TAG, "no mem for multi stage encoders");
        memset(multi_encoder->encoders, 0, sizeof(rmt_multi_item_t) * numberEncoders);
    }

    *ret_encoder = &multi_encoder->base;

    return ESP_OK;
err:
    // cleanup on error
    if (multi_encoder)
    {
        platform_free(multi_encoder);
    }
    return ret;
}

esp_err_t rmt_add_byte_encoder(rmt_encoder_t *encoder, int loopCount, rmt_bytes_encoder_config_t &config)
{
    esp_err_t ret = ESP_OK;
    rmt_multi_stage_encoder_t *multi_encoder = __containerof(encoder, rmt_multi_stage_encoder_t, base);
    rmt_encoder_handle_t byte_encoder;
    rmt_multi_item_t *mstep;

    ESP_GOTO_ON_ERROR(rmt_new_bytes_encoder(&config, &byte_encoder), err, TAG, "create bytes encoder failed");

    mstep = multi_encoder->encoders + multi_encoder->step;

    mstep->encType = RmtEncoderType_Byte;
    mstep->encoder = byte_encoder;
    mstep->currentLoop = 0;
    mstep->loopCount = loopCount;
    mstep->dataRequired = true;
    mstep->data = NULL;
    mstep->dataLen = 0;

    multi_encoder->step++;

err:
    return ret;
}

esp_err_t rmt_add_copy_encoder(
    rmt_encoder_t *encoder,
    int loopCount,
    rmt_copy_encoder_config_t &config,
    rmt_symbol_word_t *symbols,
    uint dataLen)
{
    esp_err_t ret = ESP_OK;
    rmt_multi_stage_encoder_t *multi_encoder = __containerof(encoder, rmt_multi_stage_encoder_t, base);
    rmt_encoder_handle_t copy_encoder;
    rmt_multi_item_t *mstep;

    ESP_GOTO_ON_ERROR(rmt_new_copy_encoder(&config, &copy_encoder), err, TAG, "create copy encoder failed");

    mstep = multi_encoder->encoders + multi_encoder->step;
    mstep->encType = RmtEncoderType_Copy;
    mstep->encoder = copy_encoder;
    mstep->loopCount = loopCount;
    mstep->currentLoop = 0;

    mstep->data = (void *)symbols;
    mstep->dataLen = dataLen;
    mstep->dataRequired = (symbols == NULL) ? true : false;

    multi_encoder->step++;
err:
    return ret;
}
