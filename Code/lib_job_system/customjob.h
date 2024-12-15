#include <string>
#include "job.h"

class CustomJob : public Job
{
    public: 
        CustomJob(unsigned long jobChannels, int jobType) : Job(jobChannels, jobType) {};
        ~CustomJob() {};
        
        std::string output;
        int returnCode;

        void execute();
        void jobCompletedCallBack();
};