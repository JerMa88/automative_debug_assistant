#include <dlfcn.h>
#include <iostream>
#include <string>

#include "./lib_job_system/jobsystemapi.h"
#include "fsparser.h"

int main()
{
    JobSystemAPI* jsAPI = new JobSystemAPI();
    std::cout << "Initiate JobSystem!\n";

    jsAPI->StartOrGetJobSystem();
    
    std::cout << "Creating Root Worker Thread\n";

    jsAPI->CreateWorkerThread("{\"workerThreadName\":\"RootThread0\",\"workerJobChannels\":\"FFFFFFFF\"}");
    
    std::cout << "Register FSGenJob Type\n";
    jsAPI->RegisterJobType("{\"jobType\":\"FSGenJob\",\"jobLibrary\":\"./Data/libFetchFSGenJob.so\"}");
    jsAPI->RegisterJobType("{\"jobType\":\"CompileJob\",\"jobLibrary\":\"./Data/libFetchCompileJob.so\"}");
    jsAPI->RegisterJobType("{\"jobType\":\"ParseJob\",\"jobLibrary\":\"./Data/libFetchParseJob.so\"}");
    jsAPI->RegisterJobType("{\"jobType\":\"OutputJob\",\"jobLibrary\":\"./Data/libFetchOutputJob.so\"}");
    jsAPI->RegisterJobType("{\"jobType\":\"LLMJob\",\"jobLibrary\":\"./Data/libFetchLLMJob.so\"}");
    jsAPI->RegisterJobType("{\"jobType\":\"CustomJob\",\"jobLibrary\":\"./Data/libFetchCustomJob.so\"}");

    std::cout << "Creating FSGenJob:\n";
    jsAPI->CreateJob("{\"jobType\":\"FSGenJob\",\"jobChannels\":\"FFFFFFFF\"}");

    std::ifstream code("./Data/return_code.txt");
    std::string returnCode;
    code >> returnCode;
    int rc = std::stoi(returnCode);
    FSParser p;

    while(rc != 0)
    {
        std::ifstream code("./Data/return_code.txt");
        p.parseDotFile("./Data/flowscrpt.dot");
        code >> returnCode;
        int rc = std::stoi(returnCode);
    }
    
    code.close();

    return 0;
}