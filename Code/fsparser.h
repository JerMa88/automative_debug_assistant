#include <iostream>
#include <fstream>
#include <sstream>
#include <dlfcn.h>
#include <string>
#include <deque>

#include "./lib_job_system/jobsystemapi.h"

class FSParser
{
private:
    size_t threadCount = 0;
    JobSystemAPI* jsAPI;
    std::deque <std::string> oldFunc;

public:

    void parseDotFile(std::string filePath);
};
