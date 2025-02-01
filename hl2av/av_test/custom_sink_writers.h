
#pragma once

#include <memory>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <vector>
#include "custom_media_types.h"
#include "custom_media_sink.h"

class CustomSinkWriter
{
private:
    CustomMediaSink* m_pSink; // Release
    IMFSinkWriter*   m_pSinkWriter; // Release
    DWORD            m_dwStreamIndex;

public:
    CustomSinkWriter(HOOK_SINK_PROC hookproc, void* hookparam, IMFMediaType* pInputType, IMFMediaType* pOutputType, IMFAttributes* pEncoderAttr);
    virtual ~CustomSinkWriter();

    void WriteSample(IMFSample* pSample);

    static std::unique_ptr<CustomSinkWriter> CreateForAudio(HOOK_SINK_PROC hookproc, void* hookparam, AudioSubtype input_subtype, AACFormat  const& format);
    static std::unique_ptr<CustomSinkWriter> CreateForVideo(HOOK_SINK_PROC hookproc, void* hookparam, VideoSubtype input_subtype, H26xFormat const& format, uint32_t stride, std::vector<uint64_t> const& encoder_options);
};
