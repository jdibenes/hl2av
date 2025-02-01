
#include "../av_test/core.h"
#include "../av_test/sample.h"
#include "../av_test/encoder.h"
#include "../av_test/decoder.h"

#define HL2AV_EXPORT extern "C" __declspec(dllexport)

//-----------------------------------------------------------------------------
// Core
//-----------------------------------------------------------------------------

HL2AV_EXPORT
void hl2av_core_startup()
{
    Core_Startup();
}

HL2AV_EXPORT
void hl2av_core_shutdown()
{
    Core_Shutdown();
}

//-----------------------------------------------------------------------------
// Sample
//-----------------------------------------------------------------------------

HL2AV_EXPORT
void* hl2av_sample_create(void*& base, uint32_t size)
{
    return Sample_Create(base, size);
}

HL2AV_EXPORT
void hl2av_sample_setinfo(void* self, int64_t time, int64_t duration)
{
    Sample_SetInfo(self, time, duration);
}

HL2AV_EXPORT
void hl2av_sample_getbase(void* self, void*& base, uint32_t& size)
{
    Sample_GetBase(self, base, size);
}

HL2AV_EXPORT
void hl2av_sample_getinfo(void* self, int64_t& time, int64_t& duration, uint32_t& cleanpoint)
{
    Sample_GetInfo(self, time, duration, cleanpoint);
}

HL2AV_EXPORT
void hl2av_sample_release(void* self)
{
    Sample_Release(self);
}

//-----------------------------------------------------------------------------
// Encoder
//-----------------------------------------------------------------------------

HL2AV_EXPORT
void* hl2av_encoder_createforvideo(uint16_t width, uint16_t stride, uint16_t height, uint8_t framerate, uint8_t divisor, uint8_t subtype, uint8_t profile, int8_t level, uint32_t bitrate)
{
    return Encoder_CreateForVideo(width, stride, height, framerate, divisor, (VideoSubtype)subtype, (H26xProfile)profile, level, bitrate);
}

HL2AV_EXPORT
void* hl2av_encoder_createforaudio(uint8_t channels, uint16_t samplerate, uint8_t subtype, uint8_t profile, uint8_t level)
{
    return Encoder_CreateForAudio(channels, samplerate, (AudioSubtype)subtype, (AACProfile)profile, (AACLevel)level);
}

HL2AV_EXPORT
void hl2av_encoder_push(void* self, void* sample)
{
    Encoder_Push(self, sample);
}

HL2AV_EXPORT
uint8_t hl2av_encoder_peek(void* self)
{
    return Encoder_Peek(self);
}

HL2AV_EXPORT
void* hl2av_encoder_pull(void* self)
{
    return Encoder_Pull(self);
}

HL2AV_EXPORT
void hl2av_encoder_destroy(void* self)
{
    Encoder_Destroy(self);
}

//-----------------------------------------------------------------------------
// Decoder
//-----------------------------------------------------------------------------

HL2AV_EXPORT
void* hl2av_decoder_createforvideo(uint8_t profile, uint8_t subtype)
{
    return Decoder_CreateForVideo((H26xProfile)profile, (VideoSubtype)subtype);
}

HL2AV_EXPORT
void* hl2av_decoder_createforaudio(uint8_t channels, uint16_t samplerate, uint8_t profile, uint8_t subtype)
{
    return Decoder_CreateForAudio(channels, samplerate, (AACProfile)profile, (AudioSubtype)subtype);
}

HL2AV_EXPORT
void hl2av_decoder_push(void* self, void* sample)
{
    Decoder_Push(self, sample);
}

HL2AV_EXPORT
bool hl2av_decoder_peek(void* self)
{
    return Decoder_Peek(self);
}

HL2AV_EXPORT
void* hl2av_decoder_pull(void* self)
{
    return Decoder_Pull(self);
}

HL2AV_EXPORT
void hl2av_decoder_destroy(void* self)
{
    Decoder_Destroy(self);
}
