
#pragma once

#include <mfidl.h>

class CustomMediaStream : public IMFMediaStream
{
private:
    ULONG                m_nRefCount;
    IMFMediaSource*      m_pMediaSource; // Release
    IMFStreamDescriptor* m_pStreamDescriptor; // Release
    IMFMediaEventQueue*  m_pEventQueue; // Release

    CustomMediaStream(IMFMediaSource* pMediaSource, DWORD dwStreamIdentifier, IMFMediaType* pMediaType);
    ~CustomMediaStream();

public:
    static HRESULT CreateInstance(CustomMediaStream** ppStream, IMFMediaSource* pMediaSource, DWORD dwStreamIdentifier, IMFMediaType* pMediaType);

    // IUnknown Methods
    ULONG   AddRef();
    ULONG   Release();
    HRESULT QueryInterface(REFIID iid, void** ppv);

    // IMFMediaEventGenerator Methods
    HRESULT BeginGetEvent(IMFAsyncCallback* pCallback, IUnknown* punkState);
    HRESULT EndGetEvent(IMFAsyncResult* pResult, IMFMediaEvent** ppEvent);
    HRESULT GetEvent(DWORD dwFlags, IMFMediaEvent** ppEvent);
    HRESULT QueueEvent(MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, const PROPVARIANT* pvValue);

    // IMFMediaStream Methods
    HRESULT GetMediaSource(IMFMediaSource** ppMediaSource);
    HRESULT GetStreamDescriptor(IMFStreamDescriptor** ppStreamDescriptor);
    HRESULT RequestSample(IUnknown* pToken);

    // CustomMediaStream Events
    void Shutdown();
    void Start(const GUID* pguidTimeFormat, const PROPVARIANT* pvarStartPosition);
    void Stop();
    void Pause();
};
