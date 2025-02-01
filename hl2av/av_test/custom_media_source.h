
#pragma once

#include "custom_media_stream.h"

typedef void(*HOOK_SOURCE_PROC)(IMFSample**, void*);

class CustomMediaSource : public IMFMediaSource
{
private:
    ULONG                      m_nRefCount;
    DWORD                      m_dwCharacteristics;
    IMFMediaEventQueue*        m_pEventQueue; // Release    
    CustomMediaStream*         m_stream; // Release
    IMFPresentationDescriptor* m_pPresentationDescriptor; // Release
    HOOK_SOURCE_PROC           m_pHookCallback;
    void*                      m_pHookParam;

    CustomMediaSource(DWORD dwCharacteristics, IMFMediaType* pMediaType, HOOK_SOURCE_PROC pHookCallback, void* pHookParam);
    ~CustomMediaSource();

public:
    static HRESULT CreateInstance(CustomMediaSource** ppSource, DWORD dwCharacteristics, IMFMediaType* pMediaType, HOOK_SOURCE_PROC pHookCallback, void* pHookParam);

    void InvokeHook(IMFSample** ppSample);

    // IUnknown Methods
    ULONG   AddRef();
    ULONG   Release();
    HRESULT QueryInterface(REFIID iid, void** ppv);

    // IMFMediaEventGenerator Methods
    HRESULT BeginGetEvent(IMFAsyncCallback* pCallback, IUnknown* punkState);
    HRESULT EndGetEvent(IMFAsyncResult* pResult, IMFMediaEvent** ppEvent);
    HRESULT GetEvent(DWORD dwFlags, IMFMediaEvent** ppEvent);
    HRESULT QueueEvent(MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, const PROPVARIANT* pvValue);

    // IMFMediaSource Methods
    HRESULT CreatePresentationDescriptor(IMFPresentationDescriptor** ppPresentationDescriptor);
    HRESULT GetCharacteristics(DWORD* pdwCharacteristics);
    HRESULT Pause();
    HRESULT Shutdown();
    HRESULT Start(IMFPresentationDescriptor* pPresentationDescriptor, const GUID* pguidTimeFormat, const PROPVARIANT* pvarStartPosition);
    HRESULT Stop();
};
