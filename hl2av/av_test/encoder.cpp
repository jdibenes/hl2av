
#include "custom_encoder.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
void* Encoder_CreateForVideo(uint16_t width, uint16_t stride, uint16_t height, uint8_t framerate, uint8_t divisor, VideoSubtype subtype, H26xProfile profile, int8_t level, uint32_t bitrate)
{
    H26xFormat format;
    format.width     = width;
    format.height    = height;
    format.framerate = framerate;
    format.divisor   = divisor;
    format.profile   = profile;
    format.level     = level;
    format.bitrate   = bitrate;
    return new CustomEncoder(subtype, format, stride, {});
}

// OK
void* Encoder_CreateForAudio(uint8_t channels, uint16_t samplerate, AudioSubtype subtype, AACProfile profile, AACLevel level)
{
    AACFormat format;
    format.channels   = channels;
    format.samplerate = samplerate;
    format.profile    = profile;
    format.level      = level;
    return new CustomEncoder(subtype, format);
}

// OK
void Encoder_Push(void* self, void* sample)
{
    ((CustomEncoder*)self)->Push((IMFSample*)sample);
}

// OK
bool Encoder_Peek(void* self)
{
    return ((CustomEncoder*)self)->Peek();
}

// OK
void* Encoder_Pull(void* self)
{
    IMFSample* pSample;
    ((CustomEncoder*)self)->Pull(&pSample);
    return pSample;
}

// OK
void Encoder_Destroy(void* self)
{
    delete (CustomEncoder*)self;
}
