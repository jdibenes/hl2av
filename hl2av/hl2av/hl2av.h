
#pragma once

#include <stdint.h>

#define HL2AV_IMPORT extern "C" __declspec(dllimport)

//-----------------------------------------------------------------------------
// Core
//-----------------------------------------------------------------------------

HL2AV_IMPORT
void hl2av_core_startup();

HL2AV_IMPORT
void hl2av_core_shutdown();

//-----------------------------------------------------------------------------
// Sample
//-----------------------------------------------------------------------------

HL2AV_IMPORT
void* hl2av_sample_create(void*& base, uint32_t size);

HL2AV_IMPORT
void hl2av_sample_setinfo(void* self, int64_t time, int64_t duration);

HL2AV_IMPORT
void hl2av_sample_getbase(void* self, void*& base, uint32_t& size);

HL2AV_IMPORT
void hl2av_sample_getinfo(void* self, int64_t& time, int64_t& duration, uint32_t& cleanpoint);

HL2AV_IMPORT
void hl2av_sample_release(void* self);

//-----------------------------------------------------------------------------
// Encoder
//-----------------------------------------------------------------------------

HL2AV_IMPORT
void* hl2av_encoder_createforvideo(uint16_t width, uint16_t stride, uint16_t height, uint8_t framerate, uint8_t divisor, uint8_t subtype, uint8_t profile, int8_t level, uint32_t bitrate);

HL2AV_IMPORT
void* hl2av_encoder_createforaudio(uint8_t channels, uint16_t samplerate, uint8_t subtype, uint8_t profile, uint8_t level);

HL2AV_IMPORT
void hl2av_encoder_push(void* self, void* sample);

HL2AV_IMPORT
uint8_t hl2av_encoder_peek(void* self);

HL2AV_IMPORT
void* hl2av_encoder_pull(void* self);

HL2AV_IMPORT
void hl2av_encoder_destroy(void* self);

//-----------------------------------------------------------------------------
// Decoder
//-----------------------------------------------------------------------------

HL2AV_IMPORT
void* hl2av_decoder_createforvideo(uint8_t profile, uint8_t subtype);

HL2AV_IMPORT
void* hl2av_decoder_createforaudio(uint8_t channels, uint16_t samplerate, uint8_t profile, uint8_t subtype);

HL2AV_IMPORT
void hl2av_decoder_push(void* self, void* sample);

HL2AV_IMPORT
bool hl2av_decoder_peek(void* self);

HL2AV_IMPORT
void* hl2av_decoder_pull(void* self);

HL2AV_IMPORT
void hl2av_decoder_destroy(void* self);
