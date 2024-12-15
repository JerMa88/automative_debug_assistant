#pragma once
#include <string>
#include "job.h"

class FSGenJob : public Job
{
public:
    FSGenJob(unsigned long jobChannels, int jobType) : Job(jobChannels, jobType) {};
    ~FSGenJob() {};

    void execute();
    void jobCompletedCallBack();
};