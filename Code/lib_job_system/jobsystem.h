#pragma once
#include <mutex>
#include <deque>
#include <map>
#include <vector>
#include <thread>

#include "job.h"

constexpr int JOB_TYPE_ANY = -1;

class WorkerThread;

enum JobStatus
{
    JOB_STATUS_NEVER_SEEN,
    JOB_STATUS_QUEUED,
    JOB_STATUS_NEVER_QUEUED,
    JOB_STATUS_RUNNING,
    JOB_STATUS_COMPLETED,
    JOB_STATUS_RETIRED,
    NUM_JOB_STATUSES
};

struct JobHistoryEntry
{
    JobHistoryEntry(int jobType, JobStatus jobStatus)
        : m_jobType(jobType)
        , m_jobStatus(jobStatus){}

        int m_jobType = -1;
        int m_jobStatus = JOB_STATUS_NEVER_SEEN;
};

class JobSystem
{
    friend class WorkerThread;

public:

    static JobSystem* createOrGet();
    static void destroy();
    int totalJobs = 0;
    
    void createWorkerThread(const char* uniqueName, unsigned long workerJobChannels = 0xffffffff);
    void destroyWorkerThread(const char* uniqueName);
    void queueJob(Job* job);

    //status queries
    JobStatus getJobStatus(int jobID) const;
    bool isJobCOmplete(int jobID) const;

    void finishJob(int jobID);
    void finishCompletedJob();

private:
    JobSystem();
    ~JobSystem();
    
    Job* claimAJob(unsigned long workerJobFlags);
    void onJobCompleted(Job* jobJustExecuted);

    static JobSystem* s_jobSystem;
    std::vector<WorkerThread*>   m_workerThreads;
    mutable std::mutex          m_workerThreadsMutex;
    std::deque<Job*>            m_jobsQueued;
    std::deque<Job*>            m_jobsRunning;
    std::deque<Job*>            m_jobsCompleted;
    mutable std::mutex          m_jobsQueuedMutex;
    mutable std::mutex          m_jobsRunningMutex;
    mutable std::mutex          m_jobsCompletedMutex;

    std::vector<JobHistoryEntry>m_jobHistory;
    mutable int                 m_jobHistoryLowestActiveIndex = 0;
    mutable std::mutex          m_jobHistoryMutex;
};