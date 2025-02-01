
#pragma once

#include "custom_media_queue.h"
#include "custom_sink_writers.h"

class CustomEncoder
{
private:
    std::unique_ptr<CustomSinkWriter> m_pSinkWriter;
    std::unique_ptr<CustomMediaQueue> m_in;
    std::unique_ptr<CustomMediaQueue> m_out;
    HANDLE m_thread;

    CustomEncoder();

    void Encode();
    void ProcessSample(IMFSample* pSample);

    static DWORD WINAPI Thunk_Encode(void* self);
    static void Thunk_ProcessSample(IMFSample* pSample, void* self);

public:
    CustomEncoder(AudioSubtype input_subtype, AACFormat  const& format);
    CustomEncoder(VideoSubtype input_subtype, H26xFormat const& format, uint32_t stride, std::vector<uint64_t> const& encoder_options);

    virtual ~CustomEncoder();

    void Push(IMFSample* pSample);
    bool Peek();
    void Pull(IMFSample** ppSample);

    static std::unique_ptr<CustomEncoder> CreateForAudio(AudioSubtype input_subtype, AACFormat  const& format);
    static std::unique_ptr<CustomEncoder> CreateForVideo(VideoSubtype input_subtype, H26xFormat const& format, uint32_t stride, std::vector<uint64_t> const& encoder_options);
};
