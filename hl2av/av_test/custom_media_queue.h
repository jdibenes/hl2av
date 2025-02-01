
#pragma once

#include <Windows.h>
#include <mfidl.h>
#include <queue>

class CustomMediaQueue
{
private:
    CRITICAL_SECTION m_lock;
    HANDLE m_semaphore;
    HANDLE m_cancel;
    std::queue<IMFSample*> m_queue;

    IMFSample* Pop();

public:
    CustomMediaQueue();
    virtual ~CustomMediaQueue();

    void Push(IMFSample* pSample);
    bool Empty();
    bool Wait();
    void Cancel();
    void Pull(IMFSample** ppSample);
};
