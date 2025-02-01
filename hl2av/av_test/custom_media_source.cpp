
#include <mfapi.h>
#include "custom_media_source.h"

//-----------------------------------------------------------------------------
// CustomMediaSource Methods
//-----------------------------------------------------------------------------

// OK
HRESULT CustomMediaSource::CreateInstance(CustomMediaSource** ppSource, DWORD dwCharacteristics, IMFMediaType* pMediaType, HOOK_SOURCE_PROC pHookCallback, void* pHookParam)
{
    *ppSource = new CustomMediaSource(dwCharacteristics, pMediaType, pHookCallback, pHookParam);
    return S_OK;
}

// OK
CustomMediaSource::CustomMediaSource(DWORD dwCharacteristics, IMFMediaType* pMediaType, HOOK_SOURCE_PROC pHookCallback, void* pHookParam)
{
    IMFStreamDescriptor* pStreamDescriptor; // Release

    m_nRefCount = 1;
    m_dwCharacteristics = dwCharacteristics;
    CustomMediaStream::CreateInstance(&m_stream, this, 0, pMediaType);
    m_stream->GetStreamDescriptor(&pStreamDescriptor);
    MFCreatePresentationDescriptor(1, &pStreamDescriptor, &m_pPresentationDescriptor);
    pStreamDescriptor->Release();
    m_pPresentationDescriptor->SelectStream(0);
    MFCreateEventQueue(&m_pEventQueue);
    m_pHookCallback = pHookCallback;
    m_pHookParam = pHookParam;
}

// OK
CustomMediaSource::~CustomMediaSource()
{
    m_pEventQueue->Release();
    m_pPresentationDescriptor->Release();
}

// OK
void CustomMediaSource::InvokeHook(IMFSample** ppSample)
{
    m_pHookCallback(ppSample, m_pHookParam);
}

//-----------------------------------------------------------------------------
// IUnknown Methods
//-----------------------------------------------------------------------------

// OK
ULONG CustomMediaSource::AddRef()
{
    return InterlockedIncrement(&m_nRefCount);
}

// OK
ULONG CustomMediaSource::Release()
{
    ULONG uCount = InterlockedDecrement(&m_nRefCount);
    if (uCount == 0) { delete this; }
    return uCount;
}

// OK
HRESULT CustomMediaSource::QueryInterface(REFIID iid, void** ppv)
{
    if (!ppv) { return E_POINTER; }

    *ppv = NULL;

    if      (iid == IID_IUnknown)               { *ppv = static_cast<IUnknown*>(this); }
    else if (iid == IID_IMFMediaEventGenerator) { *ppv = static_cast<IMFMediaEventGenerator*>(this); }
    else if (iid == IID_IMFMediaSource)         { *ppv = static_cast<IMFMediaSource*>(this); }
    else                                        { return E_NOINTERFACE; }

    AddRef();
    return S_OK;
}

//-----------------------------------------------------------------------------
// IMFMediaEventGenerator Methods
//-----------------------------------------------------------------------------

// OK
HRESULT CustomMediaSource::BeginGetEvent(IMFAsyncCallback* pCallback, IUnknown* punkState)
{
    return m_pEventQueue->BeginGetEvent(pCallback, punkState);
}

// OK
HRESULT CustomMediaSource::EndGetEvent(IMFAsyncResult* pResult, IMFMediaEvent** ppEvent)
{
    return m_pEventQueue->EndGetEvent(pResult, ppEvent);
}

// OK
HRESULT CustomMediaSource::GetEvent(DWORD dwFlags, IMFMediaEvent** ppEvent)
{
    return m_pEventQueue->GetEvent(dwFlags, ppEvent);
}

// OK
HRESULT CustomMediaSource::QueueEvent(MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, const PROPVARIANT* pvValue)
{
    return m_pEventQueue->QueueEventParamVar(met, guidExtendedType, hrStatus, pvValue);
}

//-----------------------------------------------------------------------------
// IMFMediaSource Methods
//-----------------------------------------------------------------------------

// OK
HRESULT CustomMediaSource::CreatePresentationDescriptor(IMFPresentationDescriptor** ppPresentationDescriptor)
{
    (*ppPresentationDescriptor = m_pPresentationDescriptor)->AddRef();
    return S_OK;
}

// OK
HRESULT CustomMediaSource::GetCharacteristics(DWORD* pdwCharacteristics)
{
    *pdwCharacteristics = m_dwCharacteristics;
    return S_OK;
}

// OK
HRESULT CustomMediaSource::Pause()
{
    m_stream->Pause();
    return S_OK;
}

// OK
HRESULT CustomMediaSource::Shutdown()
{
    m_pEventQueue->Shutdown();
    m_stream->Shutdown();
    m_stream->Release();
    return S_OK;
}

// OK
HRESULT CustomMediaSource::Start(IMFPresentationDescriptor* pPresentationDescriptor, const GUID* pguidTimeFormat, const PROPVARIANT* pvarStartPosition)
{
    (void)pPresentationDescriptor;
    m_pEventQueue->QueueEventParamUnk(MENewStream,     GUID_NULL, S_OK, m_stream);
    m_pEventQueue->QueueEventParamVar(MESourceStarted, GUID_NULL, S_OK, pvarStartPosition);
    m_stream->Start(pguidTimeFormat, pvarStartPosition);
    return S_OK;
}

// OK
HRESULT CustomMediaSource::Stop()
{
    m_stream->Stop();
    return S_OK;
}
