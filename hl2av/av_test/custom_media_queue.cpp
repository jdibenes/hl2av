
#include "custom_media_queue.h"
#include "lock.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
CustomMediaQueue::CustomMediaQueue()
{
    InitializeCriticalSection(&m_lock);
    m_semaphore = CreateSemaphore(NULL, 0, MAXLONG, NULL);
    m_cancel = CreateEvent(NULL, TRUE, FALSE, NULL);
}

// OK
CustomMediaQueue::~CustomMediaQueue()
{
    CloseHandle(m_cancel);
    CloseHandle(m_semaphore);
    DeleteCriticalSection(&m_lock);
    while (!m_queue.empty()) { Pop()->Release(); }
}

// OK
IMFSample* CustomMediaQueue::Pop()
{
    IMFSample* pSample = m_queue.front();
    m_queue.pop();
    return pSample;
}

// OK
void CustomMediaQueue::Push(IMFSample* pSample)
{
    pSample->AddRef();
    CriticalSection cs(&m_lock);
    m_queue.push(pSample);
    ReleaseSemaphore(m_semaphore, 1, NULL);
}

// OK
bool CustomMediaQueue::Empty()
{
    CriticalSection cs(&m_lock);
    return m_queue.empty();
}

// OK
bool CustomMediaQueue::Wait()
{
    HANDLE h[] = { m_semaphore, m_cancel };
    return WaitForMultipleObjects(sizeof(h) / sizeof(HANDLE), h, FALSE, INFINITE) == WAIT_OBJECT_0;
}

// OK
void CustomMediaQueue::Cancel()
{
    SetEvent(m_cancel);
}

// OK
void CustomMediaQueue::Pull(IMFSample** ppSample)
{
    CriticalSection cs(&m_lock);
    *ppSample = Pop();
}
