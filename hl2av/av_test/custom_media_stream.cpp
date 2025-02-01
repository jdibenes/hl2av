
#include <mfapi.h>
#include "custom_media_source.h"

//-----------------------------------------------------------------------------
// CustomMediaStream Methods
//-----------------------------------------------------------------------------

// OK
HRESULT CustomMediaStream::CreateInstance(CustomMediaStream** ppStream, IMFMediaSource* pMediaSource, DWORD dwStreamIdentifier, IMFMediaType* pMediaType)
{
    *ppStream = new CustomMediaStream(pMediaSource, dwStreamIdentifier, pMediaType);
    return S_OK;
}

// OK
CustomMediaStream::CustomMediaStream(IMFMediaSource* pMediaSource, DWORD dwStreamIdentifier, IMFMediaType* pMediaType)
{
    IMFMediaTypeHandler* pMediaTypeHandler; // Release

    m_nRefCount = 1;
    (m_pMediaSource = pMediaSource)->AddRef();
    MFCreateStreamDescriptor(dwStreamIdentifier, 1, &pMediaType, &m_pStreamDescriptor);
    m_pStreamDescriptor->GetMediaTypeHandler(&pMediaTypeHandler);
    pMediaTypeHandler->SetCurrentMediaType(pMediaType);
    pMediaTypeHandler->Release();    
    MFCreateEventQueue(&m_pEventQueue);
}

// OK
CustomMediaStream::~CustomMediaStream()
{
    m_pEventQueue->Release();
    m_pStreamDescriptor->Release();
    m_pMediaSource->Release();
}

//-----------------------------------------------------------------------------
// IUnknown Methods
//-----------------------------------------------------------------------------

// OK
ULONG CustomMediaStream::AddRef()
{
    return InterlockedIncrement(&m_nRefCount);
}

// OK
ULONG CustomMediaStream::Release()
{
    ULONG uCount = InterlockedDecrement(&m_nRefCount);
    if (uCount == 0) { delete this; }
    return uCount;
}

// OK
HRESULT CustomMediaStream::QueryInterface(REFIID iid, void** ppv)
{
    if (!ppv) { return E_POINTER; }

    *ppv = NULL;

    if      (iid == IID_IUnknown)               { *ppv = static_cast<IUnknown*>(this); }
    else if (iid == IID_IMFMediaEventGenerator) { *ppv = static_cast<IMFMediaEventGenerator*>(this); }
    else if (iid == IID_IMFMediaStream)         { *ppv = static_cast<IMFMediaStream*>(this); }
    else                                        { return E_NOINTERFACE; }

    AddRef();
    return S_OK;
}

//-----------------------------------------------------------------------------
// IMFMediaEventGenerator Methods
//-----------------------------------------------------------------------------

// OK
HRESULT CustomMediaStream::BeginGetEvent(IMFAsyncCallback* pCallback, IUnknown* punkState)
{
    return m_pEventQueue->BeginGetEvent(pCallback, punkState);
}

// OK
HRESULT CustomMediaStream::EndGetEvent(IMFAsyncResult* pResult, IMFMediaEvent** ppEvent)
{
    return m_pEventQueue->EndGetEvent(pResult, ppEvent);
}

// OK
HRESULT CustomMediaStream::GetEvent(DWORD dwFlags, IMFMediaEvent** ppEvent)
{
    return m_pEventQueue->GetEvent(dwFlags, ppEvent);
}

// OK
HRESULT CustomMediaStream::QueueEvent(MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, const PROPVARIANT* pvValue)
{
    return m_pEventQueue->QueueEventParamVar(met, guidExtendedType, hrStatus, pvValue);
}

//-----------------------------------------------------------------------------
// IMFMediaStream Methods
//-----------------------------------------------------------------------------

// OK
HRESULT CustomMediaStream::GetMediaSource(IMFMediaSource** ppMediaSource)
{
    (*ppMediaSource = m_pMediaSource)->AddRef();
    return S_OK;
}

// OK
HRESULT CustomMediaStream::GetStreamDescriptor(IMFStreamDescriptor** ppStreamDescriptor)
{
    (*ppStreamDescriptor = m_pStreamDescriptor)->AddRef();
    return S_OK;
}

// OK
HRESULT CustomMediaStream::RequestSample(IUnknown* pToken)
{
    if (!pToken) { return S_OK; }
    IMFSample* pSample; // Release
    static_cast<CustomMediaSource*>(m_pMediaSource)->InvokeHook(&pSample);
    if (!pSample) { return S_OK; }
    pSample->SetUnknown(MFSampleExtension_Token, pToken);
    m_pEventQueue->QueueEventParamUnk(MEMediaSample, GUID_NULL, S_OK, pSample);
    pSample->Release();
    return S_OK;
}

//-----------------------------------------------------------------------------
// CustomMediaStream Events
//-----------------------------------------------------------------------------

// OK
void CustomMediaStream::Shutdown()
{
    m_pEventQueue->Shutdown();
}

// OK
void CustomMediaStream::Start(const GUID* pguidTimeFormat, const PROPVARIANT* pvarStartPosition)
{
    (void)pguidTimeFormat;
    QueueEvent(MEStreamStarted, GUID_NULL, S_OK, pvarStartPosition);
}

// OK
void CustomMediaStream::Stop()
{
}

// OK
void CustomMediaStream::Pause()
{
}
