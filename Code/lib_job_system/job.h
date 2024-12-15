#pragma once // #include this file once in a compilation unit
#include <atomic>

class Job
{
    friend class JobSystem;     // To Create/Destroy Job System
    friend class WorkerThread;  // To Create/Destroy Threads

public: 
    Job(unsigned long jobChannels = 0xffffffff, int jobType = -1):
        m_jobChannels(jobChannels), m_jobType(jobType)
    {
        //static std::atomic<int> s_nextJobID = 0;
        static std::atomic<int> s_nextJobID;
        m_jobID = s_nextJobID++;
    }

    virtual ~Job() {};

    virtual void execute() = 0; // pure virtual function: compiler awaiting override
    virtual void jobCompletedCallBack() {};
    int getUniqueID() const {return m_jobID;}
    void setJobChannels(unsigned long jobChannels) 
    {
        if (jobChannels != m_jobChannels)    
            m_jobChannels = jobChannels;
    }
    Job* then(Job* nextJob)
    {
        this->nextJob = nextJob;
        return nextJob;
    } // compileJob->then(parseJob)->then(outputJob);

private:
    int m_jobID = -1;
    int m_jobType = -1;

    Job* nextJob = nullptr;
    
    unsigned long m_jobChannels = 0xffffffff;
};