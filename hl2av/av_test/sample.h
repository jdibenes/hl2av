
#pragma once

#include <stdint.h>

void* Sample_Create(void*& base, uint32_t size);
void Sample_SetInfo(void* self, int64_t time, int64_t duration);
void Sample_GetBase(void* self, void*& base, uint32_t& size);
void Sample_GetInfo(void* self, int64_t& time, int64_t& duration, uint32_t& cleanpoint);
void Sample_Release(void* self);
