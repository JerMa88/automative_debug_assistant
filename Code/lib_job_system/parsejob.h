#include <vector>
#include "job.h"

class ParseJob : public Job
{
public:
    ParseJob(unsigned long jobChannels, int jobType) : Job(jobChannels, jobType) {};
    ~ParseJob(){};

    void execute();
    void jobCompletedCallBack();
};