#include <iostream>

#include "customjob.h"
#include "jobsystem.h"

void CustomJob::execute()
{
    std::cout << "CustomJob executing: user can customize this job!\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void CustomJob::jobCompletedCallBack()
{
    std::cout << "CustomJob completed\n";
}

extern "C" Job* CreateCustomJob()
{
    return new CustomJob(0xffffffff, 1);
}