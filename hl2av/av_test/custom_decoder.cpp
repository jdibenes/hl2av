
#include "custom_decoder.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
CustomDecoder::CustomDecoder()
{
    m_in  = std::make_unique<CustomMediaQueue>();
    m_out = std::make_unique<CustomMediaQueue>();

    m_thread = CreateThread(NULL, 0, Thunk_Decode, this, 0, NULL);
}

// OK
CustomDecoder::CustomDecoder(AACFormat  const& format, AudioSubtype input_subtype) : CustomDecoder()
{
    m_pSourceReader = CustomSourceReader::CreateForAudio(Thunk_RequestSample, this, format, input_subtype);
}

// OK
CustomDecoder::CustomDecoder(H26xFormat const& format, VideoSubtype input_subtype) : CustomDecoder()
{
    m_pSourceReader = CustomSourceReader::CreateForVideo(Thunk_RequestSample, this, format, input_subtype);
}

// OK
CustomDecoder::~CustomDecoder()
{
    m_in->Cancel();
    m_out->Cancel();

    WaitForSingleObject(m_thread, INFINITE);
    CloseHandle(m_thread);
}

// OK
void CustomDecoder::Push(IMFSample* pSample)
{
    if (pSample) { m_in->Push(pSample); }
}

// OK
bool CustomDecoder::Peek()
{
    return !m_out->Empty();
}

// OK
void CustomDecoder::Pull(IMFSample** ppSample)
{
    if (m_out->Wait()) { m_out->Pull(ppSample); } else { *ppSample = NULL; }
}

// OK
void CustomDecoder::Decode()
{
    IMFSample* pSample; // Release

    while (m_in->Wait())
    {
    m_pSourceReader->ReadSample(&pSample);
    m_out->Push(pSample);
    pSample->Release();
    }

    m_pSourceReader = nullptr;
}

// OK
void CustomDecoder::RequestSample(IMFSample** ppSample)
{
    m_in->Pull(ppSample);
}

// OK
DWORD WINAPI CustomDecoder::Thunk_Decode(void* self)
{
    static_cast<CustomDecoder*>(self)->Decode();
    return 0;
}

// OK
void CustomDecoder::Thunk_RequestSample(IMFSample** ppSample, void* self)
{
    static_cast<CustomDecoder*>(self)->RequestSample(ppSample);
}

// OK
std::unique_ptr<CustomDecoder> CustomDecoder::CreateForAudio(AACFormat  const& format, AudioSubtype input_subtype)
{
    return std::make_unique<CustomDecoder>(format, input_subtype);
}

// OK
std::unique_ptr<CustomDecoder> CustomDecoder::CreateForVideo(H26xFormat const& format, VideoSubtype input_subtype)
{
    return std::make_unique<CustomDecoder>(format, input_subtype);
}
