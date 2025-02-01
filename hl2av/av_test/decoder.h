
#pragma once

#include "custom_media_types.h"

void* Decoder_CreateForVideo(H26xProfile profile, VideoSubtype subtype);
void* Decoder_CreateForAudio(uint8_t channels, uint16_t samplerate, AACProfile profile, AudioSubtype subtype);
void Decoder_Push(void* self, void* sample);
bool Decoder_Peek(void* self);
void* Decoder_Pull(void* self);
void Decoder_Destroy(void* self);
