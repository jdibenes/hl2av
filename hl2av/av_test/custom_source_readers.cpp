
#include <mfapi.h>
#include "custom_source_readers.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
CustomSourceReader::CustomSourceReader(HOOK_SOURCE_PROC hookproc, void* hookparam, IMFMediaType* pInputType, IMFMediaType* pOutputType)
{
    IMFAttributes* pSourceAttr; // Release

    MFCreateAttributes(&pSourceAttr, 4);

    pSourceAttr->SetUINT32(MF_LOW_LATENCY, TRUE);
    pSourceAttr->SetUINT32(MF_READWRITE_DISABLE_CONVERTERS, FALSE);
    pSourceAttr->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, TRUE);
    pSourceAttr->SetUINT32(MF_SOURCE_READER_ENABLE_ADVANCED_VIDEO_PROCESSING, TRUE);

    CustomMediaSource::CreateInstance(&m_pSource, MFMEDIASOURCE_DOES_NOT_USE_NETWORK, pInputType, hookproc, hookparam);

    MFCreateSourceReaderFromMediaSource(m_pSource, pSourceAttr, &m_pSourceReader);

    m_pSourceReader->SetCurrentMediaType(0, NULL, pOutputType);

    pSourceAttr->Release();

    m_stride = 0;
    m_width  = 0;
    m_height = 0;
}

// OK
CustomSourceReader::~CustomSourceReader()
{
    m_pSourceReader->Release();
    m_pSource->Release();
}

// OK
void CustomSourceReader::UpdateMediaType()
{
    IMFMediaType* pMediaType; // Release
    m_pSourceReader->GetCurrentMediaType(0, &pMediaType);
    pMediaType->GetUINT32(MF_MT_DEFAULT_STRIDE, &m_stride);
    MFGetAttributeSize(pMediaType, MF_MT_FRAME_SIZE, &m_width, &m_height);
    pMediaType->Release();
}

// OK
void CustomSourceReader::ReadSample(IMFSample** ppSample)
{
    IMFSample* pSample;
    DWORD dwStreamFlags;
    m_pSourceReader->ReadSample(0, 0, NULL, &dwStreamFlags, NULL, &pSample);
    if (dwStreamFlags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED) { UpdateMediaType(); }
    pSample->SetUINT32(MF_MT_DEFAULT_STRIDE, m_stride);
    MFSetAttributeSize(pSample, MF_MT_FRAME_SIZE, m_width, m_height);
    *ppSample = pSample;
}

// OK
std::unique_ptr<CustomSourceReader> CustomSourceReader::CreateForAudio(HOOK_SOURCE_PROC hookproc, void* hookparam, AACFormat const& format, AudioSubtype output_subtype)
{
    IMFMediaType* pTypeInput; // Release
    IMFMediaType* pTypeOutput; // Release

    CreatePlaceholderAudio(&pTypeInput,  format.channels, format.samplerate, output_subtype, format.profile,              format.level);
    CreatePlaceholderAudio(&pTypeOutput, format.channels, format.samplerate, output_subtype, AACProfile::AACProfile_None, AACLevel::AACLevel_Default);

    std::unique_ptr<CustomSourceReader> pSourceReader = std::make_unique<CustomSourceReader>(hookproc, hookparam, pTypeInput, pTypeOutput);

    pTypeInput->Release();
    pTypeOutput->Release();

    return pSourceReader;
}

// OK
std::unique_ptr<CustomSourceReader> CustomSourceReader::CreateForVideo(HOOK_SOURCE_PROC hookproc, void* hookparam, H26xFormat const& format, VideoSubtype output_subtype)
{
    IMFMediaType* pTypeInput; // Release
    IMFMediaType* pTypeOutput; // Release

    CreatePlaceholderVideo(&pTypeInput,  output_subtype, format.profile);
    CreatePlaceholderVideo(&pTypeOutput, output_subtype, H26xProfile::H26xProfile_None);

    std::unique_ptr<CustomSourceReader> pSourceReader = std::make_unique<CustomSourceReader>(hookproc, hookparam, pTypeInput, pTypeOutput);

    pTypeInput->Release();
    pTypeOutput->Release();

    return pSourceReader;
}
