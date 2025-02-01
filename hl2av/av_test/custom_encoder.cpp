
#include "custom_encoder.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
CustomEncoder::CustomEncoder()
{
    m_in  = std::make_unique<CustomMediaQueue>();
    m_out = std::make_unique<CustomMediaQueue>();

    m_thread = CreateThread(NULL, 0, Thunk_Encode, this, 0, NULL);
}

// OK
CustomEncoder::CustomEncoder(AudioSubtype input_subtype, AACFormat  const& format) : CustomEncoder()
{
    m_pSinkWriter = CustomSinkWriter::CreateForAudio(Thunk_ProcessSample, this, input_subtype, format);
}

// OK
CustomEncoder::CustomEncoder(VideoSubtype input_subtype, H26xFormat const& format, uint32_t stride, std::vector<uint64_t> const& encoder_options) : CustomEncoder()
{
    m_pSinkWriter = CustomSinkWriter::CreateForVideo(Thunk_ProcessSample, this, input_subtype, format, stride, encoder_options);
}

// OK
CustomEncoder::~CustomEncoder()
{
    m_in->Cancel();
    m_out->Cancel();

    WaitForSingleObject(m_thread, INFINITE);
    CloseHandle(m_thread);
}

// OK
void CustomEncoder::Push(IMFSample* pSample)
{
    if (pSample) { m_in->Push(pSample); }
}

// OK
bool CustomEncoder::Peek()
{
    return !m_out->Empty();
}

// OK
void CustomEncoder::Pull(IMFSample** ppSample)
{
    if (m_out->Wait()) { m_out->Pull(ppSample); } else { *ppSample = NULL; }
}

// OK
void CustomEncoder::Encode()
{
    IMFSample* pSample; // Release

    while (m_in->Wait())
    {
    m_in->Pull(&pSample);
    m_pSinkWriter->WriteSample(pSample);
    pSample->Release();
    }

    m_pSinkWriter = nullptr;
}

// OK
void CustomEncoder::ProcessSample(IMFSample* pSample)
{
    m_out->Push(pSample);
}

// OK
DWORD WINAPI CustomEncoder::Thunk_Encode(void* self)
{
    static_cast<CustomEncoder*>(self)->Encode();
    return 0;
}

// OK
void CustomEncoder::Thunk_ProcessSample(IMFSample* pSample, void* self)
{
    static_cast<CustomEncoder*>(self)->ProcessSample(pSample);
}

// OK
std::unique_ptr<CustomEncoder> CustomEncoder::CreateForAudio(AudioSubtype input_subtype, AACFormat  const& format)
{
    return std::make_unique<CustomEncoder>(input_subtype, format);
}

// OK
std::unique_ptr<CustomEncoder> CustomEncoder::CreateForVideo(VideoSubtype input_subtype, H26xFormat const& format, uint32_t stride, std::vector<uint64_t> const& encoder_options)
{
    return std::make_unique<CustomEncoder>(input_subtype, format, stride, encoder_options);
}
