#pragma once
#include <map>
#include "job.h"

class JobSystemAPI
{
    public:
        std::map<std::string, Job*(*)()> m_jobTypeMap;
        std::string ListJobs();
        std::string CreateJob(std::string input);
        std::string FinishCompletedJob();
        std::string DestroyJob();
        std::string GetJobStatus(int jobID);
        std::string StartOrGetJobSystem();
        std::string DestroyJobSystem();
        std::string RegisterJobType(std::string input);
        std::string CreateWorkerThread(std::string input);
        std::string LinkJobs(std::string input);
};