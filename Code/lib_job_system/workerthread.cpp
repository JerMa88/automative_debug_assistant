#include "workerthread.h"
#include "jobsystem.h"

WorkerThread::WorkerThread(const char* uniqueName, unsigned long workerJobChannels, JobSystem* jobSystem):
    m_uniqueName(uniqueName),
    m_workerJobChannels(workerJobChannels),
    m_jobSystem(jobSystem)
{}

WorkerThread::~WorkerThread()
{
    // if we have not already signal
    // thread that we should exit as soon as it can (after its current job if any)
    shutDown();

    // block on the thread's main until it has a chance to finish its current job and exit
    m_thread->join();
    delete m_thread;
    m_thread = nullptr;
}

void WorkerThread::startUp()
{
    m_thread = new std::thread(workerThreadMain, this);
}

void WorkerThread::work()
{
    while(!isStopping())
    {
        m_workerStatusMutex.lock();
        unsigned long workerJobChannels = m_workerJobChannels;
        m_workerStatusMutex.unlock();

        Job* job = m_jobSystem->claimAJob(m_workerJobChannels);
        if(job)
        {
            job->execute();
            m_jobSystem->onJobCompleted(job);
        } // do it

        std::this_thread::sleep_for(std::chrono::microseconds(1)); //rest for 1 microseconds (how fast I want a new job)
    } // do it again
}

void WorkerThread::shutDown()
{
    m_workerStatusMutex.lock();
    m_isStopping = true;
    m_workerStatusMutex.unlock();
}

bool WorkerThread::isStopping() const
{
    m_workerStatusMutex.lock();
    bool shouldClose = m_isStopping;
    m_workerStatusMutex.unlock();

    return shouldClose;
}

void WorkerThread::setWorkerJobChannels(unsigned long workerJobChannels)
{
    m_workerStatusMutex.lock();
    m_workerJobChannels = workerJobChannels;
    m_workerStatusMutex.unlock();
}

void WorkerThread::workerThreadMain(void* workThreadObject)
{
    WorkerThread* thisWorker = (WorkerThread*) workThreadObject; // cast as a JobWorkerThreadpointer
    // knows how to interpret this block of memory: size, template...
    thisWorker->work();
}