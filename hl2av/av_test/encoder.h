
#pragma once

#include "custom_media_types.h"

void* Encoder_CreateForVideo(uint16_t width, uint16_t stride, uint16_t height, uint8_t framerate, uint8_t divisor, VideoSubtype subtype, H26xProfile profile, int8_t level, uint32_t bitrate);
void* Encoder_CreateForAudio(uint8_t channels, uint16_t samplerate, AudioSubtype subtype, AACProfile profile, AACLevel level);
void Encoder_Push(void* self, void* sample);
bool Encoder_Peek(void* self);
void* Encoder_Pull(void* self);
void Encoder_Destroy(void* self);
