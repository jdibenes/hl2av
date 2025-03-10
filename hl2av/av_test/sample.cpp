
#include <mfapi.h>
#include "custom_media_types.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
void* Sample_Create(void*& base, uint32_t size)
{
    IMFMediaBuffer* pBuffer; // Release
    IMFSample* pSample;
    BYTE* address;

    MFCreateMemoryBuffer(size, &pBuffer);
    MFCreateSample(&pSample);

    pBuffer->Lock(&address, NULL, NULL);
    pBuffer->Unlock();
    pBuffer->SetCurrentLength(size);
    pSample->AddBuffer(pBuffer);
    pBuffer->Release();

    base = address;

    return pSample;
}

// OK
void Sample_SetInfo(void* self, int64_t time, int64_t duration)
{
    IMFSample* pSample = (IMFSample*)self;
    pSample->SetSampleTime(time);
    pSample->SetSampleDuration(duration);
}

// OK
void Sample_GetBase(void* self, void*& base, uint32_t& size)
{
    IMFSample* pSample = (IMFSample*)self;
    IMFMediaBuffer* pBuffer; // Release
    BYTE* address;
    pSample->ConvertToContiguousBuffer(&pBuffer);
    pBuffer->Lock(&address, NULL, (DWORD*)&size);
    pBuffer->Unlock();
    pBuffer->Release();
    base = address;
}

// OK
void Sample_GetInfo(void* self, int64_t& time, int64_t& duration, uint32_t& cleanpoint)
{
    IMFSample* pSample = (IMFSample*)self;
    pSample->GetSampleTime(&time);
    pSample->GetSampleDuration(&duration);    
    pSample->GetUINT32(MFSampleExtension_CleanPoint, &cleanpoint);
}

// OK
void Sample_GetResolution(void* self, uint32_t& width, uint32_t& stride, uint32_t& height)
{
    IMFSample* pSample = (IMFSample*)self;
    pSample->GetUINT32(MF_MT_DEFAULT_STRIDE, &stride);
    MFGetAttributeSize(pSample, MF_MT_FRAME_SIZE, &width, &height);
}

// OK
void Sample_Release(void* self)
{
    ((IMFSample*)self)->Release();
}

// OK
bool Buffer_GetAudioParameters(void* base, uint8_t& channels, uint16_t& samplerate)
{
    return TranslateADTSOptions((BYTE*)base, channels, samplerate);
}
