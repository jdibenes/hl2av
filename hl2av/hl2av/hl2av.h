
#pragma once

#include <stdint.h>

#define HL2AV_IMPORT extern "C" __declspec(dllimport)

//-----------------------------------------------------------------------------
// Core
//-----------------------------------------------------------------------------

HL2AV_IMPORT
void core_startup();

HL2AV_IMPORT
void core_shutdown();

//-----------------------------------------------------------------------------
// Sample
//-----------------------------------------------------------------------------

HL2AV_IMPORT
void* sample_create(void*& base, uint32_t size);

HL2AV_IMPORT
void sample_setinfo(void* self, int64_t time, int64_t duration);

HL2AV_IMPORT
void sample_getbase(void* self, void*& base, uint32_t& size);

HL2AV_IMPORT
void sample_getinfo(void* self, int64_t& time, int64_t& duration, uint32_t& cleanpoint);

HL2AV_IMPORT
void sample_getresolution(void* self, uint32_t& width, uint32_t& stride, uint32_t& height);

HL2AV_IMPORT
void sample_release(void* self);

//-----------------------------------------------------------------------------
// Encoder
//-----------------------------------------------------------------------------

HL2AV_IMPORT
void* encoder_createforvideo(uint16_t width, uint16_t stride, uint16_t height, uint8_t framerate, uint8_t divisor, uint8_t subtype, uint8_t profile, int8_t level, uint32_t bitrate);

HL2AV_IMPORT
void* encoder_createforaudio(uint8_t channels, uint16_t samplerate, uint8_t subtype, uint8_t profile, uint8_t level);

HL2AV_IMPORT
void encoder_push(void* self, void* sample);

HL2AV_IMPORT
uint8_t encoder_peek(void* self);

HL2AV_IMPORT
void* encoder_pull(void* self);

HL2AV_IMPORT
void encoder_destroy(void* self);

//-----------------------------------------------------------------------------
// Decoder
//-----------------------------------------------------------------------------

HL2AV_IMPORT
void* decoder_createforvideo(uint8_t profile, uint8_t subtype);

HL2AV_IMPORT
void* decoder_createforaudio(uint8_t channels, uint16_t samplerate, uint8_t profile, uint8_t subtype);

HL2AV_IMPORT
void decoder_push(void* self, void* sample);

HL2AV_IMPORT
uint8_t decoder_peek(void* self);

HL2AV_IMPORT
void* decoder_pull(void* self);

HL2AV_IMPORT
void decoder_destroy(void* self);
