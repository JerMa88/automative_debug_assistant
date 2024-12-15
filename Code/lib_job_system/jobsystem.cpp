#include <iostream>
#include <cstring>

#include "jobsystem.h"
#include "workerthread.h"

JobSystem* JobSystem::s_jobSystem = nullptr;
typedef void (*JobCallback)(Job* completedJob); // typedef for function pointer method named JobCallBack

JobSystem::JobSystem()
{
    m_jobHistory.reserve(256 * 1024);
}

JobSystem::~JobSystem()
{
    m_workerThreadsMutex.lock();
    int numWorkerThreads = (int)m_workerThreads.size();

    // FIrst, tell each worker thread to stop picking up jobs
    for(int i = 0; i < numWorkerThreads; i++)
    {
        m_workerThreads[i]->shutDown();
    }
    while(!m_workerThreads.empty())
    {
        delete m_workerThreads.back(); // deleting WorkerThread objectsow concerned with threads
        m_workerThreads.pop_back();
    }
    m_workerThreadsMutex.unlock();
}

JobSystem* JobSystem::createOrGet()
{
    if(!s_jobSystem)
    {
        s_jobSystem = new JobSystem();
    }
    return s_jobSystem;
}

void JobSystem::destroy()
{
    if(!s_jobSystem)
    {
        delete s_jobSystem;
        s_jobSystem = nullptr;
    }
}

void JobSystem::createWorkerThread(const char* uniqueName, unsigned long workerJobChannels)
{
    WorkerThread* newWorker = new WorkerThread(uniqueName, workerJobChannels, this);
    m_workerThreadsMutex.lock();
    m_workerThreads.push_back(newWorker);
    m_workerThreadsMutex.unlock();

    m_workerThreads.back()->startUp();
}

void JobSystem::destroyWorkerThread(const char* uniqueName)
{
    m_workerThreadsMutex.lock();
    WorkerThread* doomedWorker = nullptr;
    std::vector<WorkerThread*>::iterator it = m_workerThreads.begin();

    for(; it != m_workerThreads.end(); it++)
    {
        if(strcmp((*it)->m_uniqueName, uniqueName) == 0)
        {
            doomedWorker = *it;
            m_workerThreads.erase(it);
            break;
        }
    }
    m_workerThreadsMutex.unlock();

    if(doomedWorker)
    {
        doomedWorker->shutDown();
        delete doomedWorker;
    }
}

void JobSystem::queueJob(Job* job)
{
    m_jobsQueuedMutex.lock();
    
    m_jobHistoryMutex.lock();
    m_jobHistory.emplace_back(JobHistoryEntry(job->m_jobType, JOB_STATUS_QUEUED));
    m_jobHistoryMutex.unlock();

    m_jobsQueued.push_back(job);
    m_jobsQueuedMutex.unlock();
}

JobStatus JobSystem::getJobStatus(int jobID) const
{
    m_jobHistoryMutex.lock();
    
    JobStatus js = JOB_STATUS_NEVER_SEEN;
    if (jobID < (int) m_jobHistory.size() && jobID >= 0)
    {
        js = (JobStatus) (m_jobHistory[jobID].m_jobStatus);
    }

    m_jobHistoryMutex.unlock();

    return js;
}

bool JobSystem::isJobCOmplete(int jobID) const
{
    return (getJobStatus(jobID) == JOB_STATUS_COMPLETED);
}

void JobSystem::finishCompletedJob()
{
    std::deque<Job*> jobsCompleted;

    m_jobsCompletedMutex.lock();
    jobsCompleted.swap(m_jobsCompleted);
    m_jobsCompletedMutex.unlock();

    for(Job* job : jobsCompleted)
    {
        job->jobCompletedCallBack();
        m_jobHistoryMutex.lock();
        m_jobHistory[job->m_jobID].m_jobStatus = JOB_STATUS_RETIRED;
        m_jobHistoryMutex.unlock();
        delete job;
    }
}

void JobSystem::finishJob(int jobID)
{
    while(!isJobCOmplete(jobID))
    {
        JobStatus jobStatus = getJobStatus(jobID);
        if((jobStatus == JOB_STATUS_NEVER_SEEN) || (jobStatus == JOB_STATUS_RETIRED))
        {
            std::cout << "ERROR: Waiting for Job (#" << jobID << ") - no such job in jobSystem\n"; 
            return;
        }

        m_jobsCompletedMutex.lock();
        Job* thisCompletedJob = nullptr;
        for (auto jcIter = m_jobsCompleted.begin(); jcIter != m_jobsCompleted.end(); jcIter++)
        {
            Job* someCompletedJob = *jcIter;
            if (someCompletedJob->m_jobID == jobID)
            {
                thisCompletedJob = someCompletedJob;
                m_jobsCompleted.erase(jcIter);
                break;
            }
        }

        m_jobsCompletedMutex.unlock();

        if(thisCompletedJob == nullptr)
        {
            std::cout << "ERROR: Job #" << jobID << "was status complete but not found in Completed list";
        }

        thisCompletedJob->jobCompletedCallBack();

        m_jobHistoryMutex.lock();
        m_jobHistory[thisCompletedJob->m_jobID].m_jobStatus = JOB_STATUS_RETIRED;
        m_jobHistoryMutex.unlock();
        
        delete thisCompletedJob;
    }
}

void JobSystem::onJobCompleted(Job* jobJustExecuted)
{
    totalJobs++;
    m_jobsCompletedMutex.lock();
    m_jobsRunningMutex.lock();

    std::deque<Job*>::iterator runningJobItr = m_jobsRunning.begin();
    for(; runningJobItr != m_jobsRunning.end(); runningJobItr++)
    {
        if(jobJustExecuted == *runningJobItr)
        {
            m_jobHistoryMutex.lock();
            m_jobsRunning.erase(runningJobItr);
            m_jobsCompleted.push_back(jobJustExecuted);
            m_jobHistory[jobJustExecuted->m_jobID].m_jobStatus = JOB_STATUS_COMPLETED;
            m_jobHistoryMutex.unlock();
            break;
        }
    }
    m_jobsRunningMutex.unlock();
    m_jobsCompletedMutex.unlock();
    if(jobJustExecuted->nextJob != nullptr)
    {
        queueJob(jobJustExecuted->nextJob);
    }
}

Job* JobSystem::claimAJob(unsigned long workerJobChannels)
{
    m_jobsQueuedMutex.lock();
    m_jobsRunningMutex.lock();

    Job* claimedJob = nullptr;
    std::deque<Job*>::iterator queuedJobItr = m_jobsQueued.begin();
    for(; queuedJobItr != m_jobsQueued.end(); queuedJobItr++)
    {
        Job* queuedJob = *queuedJobItr;

        if((queuedJob->m_jobChannels & workerJobChannels) != 0)
        {
            claimedJob = queuedJob;

            m_jobHistoryMutex.lock();
            m_jobsQueued.erase(queuedJobItr);
            m_jobsRunning.push_back(queuedJob);
            m_jobHistory[claimedJob->m_jobID].m_jobStatus = JOB_STATUS_RUNNING;
            m_jobHistoryMutex.unlock();

            break;
        }
    }

    m_jobsRunningMutex.unlock();
    m_jobsQueuedMutex.unlock();

    return claimedJob;
}