
#pragma once

#include <stdint.h>

void* Sample_Create(void*& base, uint32_t size);
void Sample_SetInfo(void* self, int64_t time, int64_t duration);
void Sample_GetBase(void* self, void*& base, uint32_t& size);
void Sample_GetInfo(void* self, int64_t& time, int64_t& duration, uint32_t& cleanpoint);
void Sample_GetResolution(void* self, uint32_t& width, uint32_t& stride, uint32_t& height);
void Sample_Release(void* self);
bool Buffer_GetAudioParameters(void* base, uint8_t& channels, uint16_t& samplerate);
