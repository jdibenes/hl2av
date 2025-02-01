
#include "custom_decoder.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
void* Decoder_CreateForVideo(H26xProfile profile, VideoSubtype subtype)
{
    H26xFormat format;
    format.profile = profile;
    return new CustomDecoder(format, subtype);
}

// OK
void* Decoder_CreateForAudio(uint8_t channels, uint16_t samplerate, AACProfile profile, AudioSubtype subtype)
{
    AACFormat format;
    format.channels   = channels;
    format.samplerate = samplerate;
    format.profile    = profile;
    format.level      = AACLevel::AACLevel_NotSet;
    return new CustomDecoder(format, subtype);
}

// OK
void Decoder_Push(void* self, void* sample)
{
    ((CustomDecoder*)self)->Push((IMFSample*)sample);
}

// OK
bool Decoder_Peek(void* self)
{
    return ((CustomDecoder*)self)->Peek();
}

// OK
void* Decoder_Pull(void* self)
{
    IMFSample* pSample;
    ((CustomDecoder*)self)->Pull(&pSample);
    return pSample;
}

// OK
void Decoder_Destroy(void* self)
{
    delete (CustomDecoder*)self;
}
