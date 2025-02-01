
#include <mfapi.h>
#include <mfidl.h>
#include <stdint.h>

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
void Sample_Release(void* self)
{
    ((IMFSample*)self)->Release();
}
