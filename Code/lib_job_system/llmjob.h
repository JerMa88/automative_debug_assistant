#pragma once
#include <string>
#include "job.h"

class LLMJob : public Job
{
public:
    LLMJob(unsigned long jobChannels, int jobType) : Job(jobChannels, jobType) {};
    ~LLMJob() {};

    std::string output;
    int returnCode;

    void execute();
    void jobCompletedCallBack();
};