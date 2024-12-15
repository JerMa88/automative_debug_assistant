#include <cerrno>
#include <vector>
#include <algorithm>

#include "fsparser.h"
#include "./lib_job_system/jobsystemapi.h"

void FSParser::parseDotFile(std::string filePath)
{
    std::ifstream file(filePath);
    std::string line;

    FSParser p;

    JobSystemAPI *jsAPI = new JobSystemAPI();
    // std::cout << "Load Functions Complete.\n";

    while (std::getline(file, line))
    {
        if (line.find("->") != std::string::npos) // Corrected condition
        {
            size_t delimiterPos = line.find("->");
            if (delimiterPos == std::string::npos)
            {
                continue; // Skip this line if there's no "->" delimiter
            }

            std::string each = line.substr(0, delimiterPos);

            size_t first = each.find_first_not_of(' ');
            if (std::string::npos != first)
            {
                size_t last = each.find_last_not_of(' ');
                each = each.substr(first, (last - first + 1));
            }

            // Skip if the job is already processed or if it's one of the special jobs
            if (each != "CreateJob" && each != "CreateWorkerThread" && each != "LinkJobs" && std::find(oldFunc.begin(), oldFunc.end(), each) != oldFunc.end())
            {
                continue;
            }
            if(line.find("[style=bold,label=\"Link\"]") != std::string::npos)
            {
                each = "LinkJobs";
            }

            oldFunc.push_back(each);
            jsAPI->StartOrGetJobSystem();

            if (each == "Start")
            {
            }
            else if (each == "StartOrGetJobSystem")
            {
                std::cout << "Starting Job System\n";
                jsAPI->StartOrGetJobSystem();
            }
            else if (each == "CreateWorkerThread")
            {
                std::cout << "Creating Worker Thread" << threadCount << "\n";
                std::string jsonStrInput = "{\"workerThreadName\":\"WorkerThread" + std::to_string(threadCount++) + "\",\"workerJobChannels\":\"FFFFFFFF\"}";
                jsAPI->CreateWorkerThread(jsonStrInput);
            }
            else if (each == "RegisterJobType")
            {
                // Find the positions of the quotation marks
                size_t firstQuote = line.find('\"');
                size_t secondQuote = line.find('\"', firstQuote + 1);
                std::string substring;
                if (firstQuote != std::string::npos && secondQuote != std::string::npos)
                {
                    // Extract the substring between the quotation marks
                    substring = line.substr(firstQuote + 1, secondQuote - firstQuote - 1);
                }

                std::vector<std::string> jobs;
                std::string delimiter = "\\n";
                size_t pos = 0;
                std::string token;

                while ((pos = substring.find(delimiter)) != std::string::npos)
                {
                    token = substring.substr(0, pos);
                    jobs.push_back(token);
                    substring.erase(0, pos + delimiter.length());
                }
                jobs.push_back(substring); // Add the last element

                // Output the contents of the vector
                for (const auto &j : jobs)
                {
                    std::string jsonStrInput = R"({"jobType":")" + j + R"(","jobLibrary":"./Data/libFetch)" + j + R"(.so"})";
                    // std::cout << "JSON string input: " << jsonStrInput << "\nRegistering Job Type " << j << "\n";
                    jsAPI->RegisterJobType(jsonStrInput);
                }
            }
            else if (each == "ListJobs")
            {
                std::cout << "\nList of types of jobs available:" << std::endl;
                jsAPI->ListJobs();
                std::cout << std::endl;
            }
            else if (each == "CreateJob")
            {
                int num = 1;
                std::string substring = line.substr(line.find("->") + 2, line.length());
                if(line.find("[label")!= std::string::npos)
                {
                    size_t firstQuote = line.find('\"');
                    size_t secondQuote = line.find('\"', firstQuote + 1);

                    std::string number = line.substr(firstQuote + 1, secondQuote - firstQuote - 1);

                    try
                    {
                        num = std::stoi(number);
                        // Use num here
                    }
                    catch (const std::invalid_argument &ia)
                    {
                        std::cerr << "Invalid argument: " << ia.what() << '\n';
                        // Handle the case where the string is not a number
                    }
                    catch (const std::out_of_range &oor)
                    {
                        std::cerr << "Out of Range error: " << oor.what() << '\n';
                        // Handle the case where the number is too large
                    }

                    std::string labelStart = "[label = ";
                    std::string labelEnd = "]";

                    size_t startPos = substring.find(labelStart);
                    if (startPos != std::string::npos) {
                        size_t endPos = substring.find(labelEnd, startPos);
                        if (endPos != std::string::npos) {
                            // Remove the label part, including the brackets
                            substring.erase(startPos, endPos - startPos + labelEnd.length());
                        }
                    }
                    std::cout << "Creating: " << num << " " << substring << "\n";
                }
                std::string jsonStrInput = R"({"jobType":")" + substring + R"(","jobChannels":"FFFFFFFF"})";

                // std::cout << "JSON string input: " << jsonStrInput << "\nCreating Job " << substring << "\n";
                for (int i = 0; i < num; i++)
                    jsAPI->CreateJob(jsonStrInput);
            }
            else if (each == "LinkJobs")
            {
                std::string baseJob = line.substr(0, line.find("->"));
                std::string dependentJob = line.substr(line.find("->") + 2, line.find("[") - line.find("->") - 2);

                // Trim whitespace from baseJob
                size_t start = baseJob.find_first_not_of(" \t");
                size_t end = baseJob.find_last_not_of(" \t");
                baseJob = (start != std::string::npos && end != std::string::npos) ? baseJob.substr(start, end - start + 1) : "";

                // Trim whitespace and additional attributes from dependentJob
                start = dependentJob.find_first_not_of(" \t");
                end = dependentJob.find_first_of("[", start); // Find the start of any additional attributes
                dependentJob = (start != std::string::npos && end != std::string::npos) ? dependentJob.substr(start, end - start) : dependentJob.substr(start);

                std::string jsonStrInput = R"({"baseJob":")" + baseJob + R"(","dependentJob":")" + dependentJob + R"("})";

                // std::cout << "JSON string input: " << jsonStrInput << "\nLinking Jobs " << baseJob << " and " << dependentJob << "\n";
                jsAPI->LinkJobs(jsonStrInput);
            }
            else if (each == "label=\"UI while loop")
            {
                int running = 1;
                while (running)
                {
                    std::string command;
                    std::cout << "Enter stop, destroy, finish or status\n";
                    std::cin >> command;

                    if (command == "stop")
                    {
                        running = 0;
                    }
                    else if (command == "destroy")
                    {
                        // jsAPI->DestroyJob();
                        running = 0;
                    }
                    else if (command == "status")
                    {
                        jsAPI->GetJobStatus(0);
                    }
                    else if (command == "finish")
                    {
                        jsAPI->FinishCompletedJob();
                    }
                    else
                    {
                        std::cout << "Invalid Entry\n";
                    }
                }
            }
            else if (each == "DestroyJobSystem")
            {
                std::cout << "Destroying Job System\n";
                // jsAPI->DestroyJobSystem();
            }
            else 
            {
                if (line.find("if") == std::string::npos)
                std::cout << "Invalid Entry : function does not exist\n";
            }
        }
    }

    file.close();

    std::ofstream outfile;

    std::string outFilePath = "./Data/functions_executed.json";
    outfile.open(outFilePath);
    if (!outfile.is_open())
    {
        std::cerr << "Unable to open file at " << outFilePath << std::endl;
        // std::cerr << "Error: " << strerror(errno) << std::endl; // Print the system error message
    }

    outfile << "{\n\"functionsExecuted\": [\n";
    for (size_t i = 0; i < oldFunc.size(); ++i)
    {
        outfile << "    \"" << oldFunc[i] << "\"";
        if (i < oldFunc.size() - 1)
        {
            outfile << ",";
        }
        outfile << "\n";
    }
    outfile << "]\n}\n";
    outfile.close();
}