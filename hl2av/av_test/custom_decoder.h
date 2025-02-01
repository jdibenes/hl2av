
#pragma once

#include "custom_media_queue.h"
#include "custom_source_readers.h"

class CustomDecoder
{
private:
    std::unique_ptr<CustomSourceReader> m_pSourceReader;
    std::unique_ptr<CustomMediaQueue> m_in;
    std::unique_ptr<CustomMediaQueue> m_out;
    HANDLE m_thread;

    CustomDecoder();

    void Decode();
    void RequestSample(IMFSample** ppSample);

    static DWORD WINAPI Thunk_Decode(void* self);
    static void Thunk_RequestSample(IMFSample** ppSample, void* self);

public:
    CustomDecoder(AACFormat  const& format, AudioSubtype output_subtype);
    CustomDecoder(H26xFormat const& format, VideoSubtype output_subtype);

    virtual ~CustomDecoder();

    void Push(IMFSample* pSample);
    bool Peek();
    void Pull(IMFSample** ppSample);

    static std::unique_ptr<CustomDecoder> CreateForAudio(AACFormat  const& format, AudioSubtype output_subtype);
    static std::unique_ptr<CustomDecoder> CreateForVideo(H26xFormat const& format, VideoSubtype output_subtype);
};
