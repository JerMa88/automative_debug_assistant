#include <iostream>

#include "job.h"

class OutputJob : public Job
{
public:
    OutputJob(unsigned long jobChannels, int jobType) : Job(jobChannels, jobType) {};
    ~OutputJob(){};

    void execute();
    void jobCompletedCallBack();
};