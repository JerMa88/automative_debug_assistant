#pragma once
#include <mutex>
#include <deque>
#include <map>
#include <vector>
#include <thread>

#include "job.h"

class JobSystem;

class WorkerThread
{
    friend class JobSystem;

private:
    WorkerThread(const char* uniqueName, unsigned long workerJobChannels, JobSystem* jobSystem);
    ~WorkerThread();

    void startUp(); // kick off actual thread, which will call work()
    void work();    // called in private thread, blocks forever until StopWorking() is called
    void shutDown();// signal that work should stop at next opportunity

    bool isStopping() const;
    void setWorkerJobChannels(unsigned long workerJobChannels);
    static void workerThreadMain(void* workThreadObject);

private:
    const char*     m_uniqueName;
    unsigned long   m_workerJobChannels = 0xffffffff;
    bool            m_isStopping = false;
    JobSystem*      m_jobSystem = nullptr;
    std::thread*    m_thread = nullptr;
    mutable std::mutex m_workerStatusMutex;
};