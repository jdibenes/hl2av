
#pragma once

#include <memory>
#include <mfidl.h>
#include <mfreadwrite.h>
#include "custom_media_types.h"
#include "custom_media_source.h"

class CustomSourceReader
{
private:
    CustomMediaSource* m_pSource; // Release
    IMFSourceReader*   m_pSourceReader; // Release
    UINT32 m_stride;
    UINT32 m_width;
    UINT32 m_height;

    void UpdateMediaType();

public:
    CustomSourceReader(HOOK_SOURCE_PROC hookproc, void* hookparam, IMFMediaType* pInputType, IMFMediaType* pOutputType);
    virtual ~CustomSourceReader();

    void ReadSample(IMFSample** ppSample);

    static std::unique_ptr<CustomSourceReader> CreateForAudio(HOOK_SOURCE_PROC hookproc, void* hookparam, AACFormat  const& format, AudioSubtype output_subtype);
    static std::unique_ptr<CustomSourceReader> CreateForVideo(HOOK_SOURCE_PROC hookproc, void* hookparam, H26xFormat const& format, VideoSubtype output_subtype);
};
