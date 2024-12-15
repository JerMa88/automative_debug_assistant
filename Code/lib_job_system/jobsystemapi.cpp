#include <dlfcn.h>
#include <iostream>
#include <string>
#include <map>
#include <sstream>

#include "./rapidjson/document.h"
#include "./jobsystem.h"
#include "./compilejob.h"
#include "./jobsystemapi.h"

// Get list of available job types
std::string JobSystemAPI::ListJobs()
{
    std::stringstream ss;
    ss << "Available Job Types:" << std::endl;
    for (std::map<std::string, Job *(*)()>::iterator it = m_jobTypeMap.begin(); it != m_jobTypeMap.end(); ++it)
    {
        ss << it->first << std::endl;
        std::cout << it->first << std::endl;
    }
    return ss.str();
}

std::string JobSystemAPI::CreateJob(std::string input)
{
    //{\"jobType\":\"CompileJob\",\"jobChannels\":\"OxFFFFFFFF\"}
    std::string jobType;
    std::string jobChannels;

    // Parse the JSON string using RapidJSON
    rapidjson::Document document;
    document.Parse(input.c_str());

    // Check if parsed successfully
    if (document.HasParseError())
    {
        std::cerr << "Error parsing the JSON string." << std::endl;
        return "1";
    }
    // Extracting values
    if (document.HasMember("jobType") && document["jobType"].IsString())
    {
        jobType = document["jobType"].GetString();
    }
    if (document.HasMember("jobChannels") && document["jobChannels"].IsString())
    {
        jobChannels = document["jobChannels"].GetString();
    }
    // Before using m_jobTypeMap[jobType]
    // std::cout << "Checking if jobType '" << jobType << "' exists in the map..." << std::endl;
    if (m_jobTypeMap.find(jobType) == m_jobTypeMap.end()) {
        std::cerr << "Error: jobType '" << jobType << "' not found in the map: NEVER REGISTERED." << std::endl;
        return nullptr; // or handle the error as appropriate
    }

    // Get the function pointer
    auto createJobFunc = m_jobTypeMap[jobType];
    if (!createJobFunc) {
        std::cerr << "Error: Function pointer for jobType '" << jobType << "' is null." << std::endl;
        return nullptr; // or handle the error as appropriate
    }

// Create an instance of the job
std::cout << "Creating job instance for jobType '" << jobType << "'." << std::endl;
Job *jobPtr = createJobFunc();
if (!jobPtr) {
    std::cerr << "Error: Failed to create job instance for jobType '" << jobType << "'." << std::endl;
    return nullptr; // or handle the error as appropriate
} else {
    std::cout << "Job instance for jobType '" << jobType << "' created successfully." << std::endl;
}

// Continue with the rest of your code...

    jobPtr->setJobChannels(stoul(jobChannels, 0, 16));
    // Queue the Job
    JobSystem *js = JobSystem::createOrGet();
    js->queueJob(jobPtr);
    return "Job Created and Queued";
}

std::string JobSystemAPI::FinishCompletedJob()
{
    JobSystem *js = JobSystem::createOrGet();
    js->finishCompletedJob();
    std::cout << "Completed Jobs Finished" << std::endl;
    return "Completed Jobs Finished";
}

std::string JobSystemAPI::DestroyJob()
{
    JobSystem *js = JobSystem::createOrGet();
    js->finishCompletedJob();
    js->destroy();
    std::cout << "Job Destroyed" << std::endl;
    return "Job Destroyed";
}

std::string JobSystemAPI::GetJobStatus(int jobID)
{
    std::cout << "Job ID: " << jobID << std::endl;
    JobSystem *js = JobSystem::createOrGet();
    std::cout << "Job System Got" << std::endl;
    int status = js->getJobStatus(jobID);
    std::cout << "Job Status: " << status << std::endl;
    std::string output = std::to_string(status);
    std::cout << "Job Status: " << output << std::endl;
    return "Job" + std::to_string(jobID) + "Status" + output;
}

std::string JobSystemAPI::StartOrGetJobSystem()
{
    JobSystem *js = JobSystem::createOrGet();
    return "Job System Started";
}

std::string JobSystemAPI::DestroyJobSystem()
{
    JobSystem *js = JobSystem::createOrGet();
    js->destroy();
    return "Job System Destroyed";
}

std::string JobSystemAPI::RegisterJobType(std::string input)
{
    std::string jobType;
    std::string jobLibrary;

    // Parse the JSON string using RapidJSON
    rapidjson::Document document;
    document.Parse(input.c_str());

    // Check if parsed successfully
    if (document.HasParseError())
    {
        std::cerr << "Error parsing the JSON string." << std::endl;
        return "1";
    }

    // Extracting values
    if (document.HasMember("jobType") && document["jobType"].IsString())
    {
        jobType = document["jobType"].GetString();
    }

    if (document.HasMember("jobLibrary") && document["jobLibrary"].IsString())
    {
        jobLibrary = document["jobLibrary"].GetString();
    }
    if (m_jobTypeMap.find(jobType) != m_jobTypeMap.end())
    {
        return "Job Type " + jobType + " Already Registered";
    }
    // Load the library
    void *handle = dlopen(jobLibrary.c_str(), RTLD_NOW);
    if (!handle)
    {
        std::cerr << "Cannot open library" << dlerror() << "\n";
        return "1";
    }

    std::string funcName = "Create" + jobType;
    // Load the function
    typedef Job *(*Func)();
    Func CreateJob = (Func)dlsym(handle, funcName.c_str());
    if (!CreateJob)
    {
        std:: cout<< "Func name:" << funcName << std::endl;
        std::cerr << "Cannot load symbol: " << funcName << dlerror() << "\n";
        dlclose(handle);
        return "1";
    }

    // Create an instance of the job

    m_jobTypeMap.insert(std::make_pair(jobType, CreateJob));
    std::cout << "Job Type " << jobType << " Registered" << std::endl;
    return "Job Type " + jobType + " Registered";
}

std::string JobSystemAPI::CreateWorkerThread(std::string input)
{
    std::string threadName;
    std::string jobChannels;

    // Parse the JSON string using RapidJSON
    rapidjson::Document document;
    document.Parse(input.c_str());

    // Check if parsed successfully
    if (document.HasParseError())
    {
        std::cerr << "Error parsing the JSON string." << std::endl;
        return "1";
    }

    // Extracting values
    if (document.HasMember("workerThreadName") && document["workerThreadName"].IsString())
    {
        threadName = document["workerThreadName"].GetString();
    }

    if (document.HasMember("workerJobChannels") && document["workerJobChannels"].IsString())
    {
        jobChannels = document["workerJobChannels"].GetString();
    }

    JobSystem *js = JobSystem::createOrGet();
    js->createWorkerThread(threadName.c_str(), stoul(jobChannels, 0, 16));
    return ("Worker Thread " + threadName + " Created");
}

std::string JobSystemAPI::LinkJobs(std::string input) {
    std::string baseJobName, dependentJobName;

    // Parse the JSON string using RapidJSON
    rapidjson::Document document;
    document.Parse(input.c_str());

    // Check if parsed successfully
    if (document.HasParseError()) {
        std::cerr << "Error parsing the JSON string." << std::endl;
        return "1";
    }

    if (document.HasMember("baseJob") && document["baseJob"].IsString()) {
        baseJobName = document["baseJob"].GetString();
    }

    if (document.HasMember("dependentJob") && document["dependentJob"].IsString()) {
        dependentJobName = document["dependentJob"].GetString();
    }

    if (m_jobTypeMap.find(baseJobName) == m_jobTypeMap.end() || 
        m_jobTypeMap.find(dependentJobName) == m_jobTypeMap.end()) {
        return "Job not found";
    }

    Job* baseJob = m_jobTypeMap[baseJobName]();
    Job* dependentJob = m_jobTypeMap[dependentJobName]();

    baseJob->then(dependentJob);

    return "Jobs Linked";
}


extern "C"
{
    JobSystemAPI *CreateJobsystemAPI()
    {
        return new JobSystemAPI();
    }

    void DestroyJobSystemAPI(JobSystemAPI *jobSystemAPI)
    {
        delete jobSystemAPI;
    }
}