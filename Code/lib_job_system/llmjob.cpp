#include <iostream>
#include <string>
#include <array>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include "rapidjson/document.h"

#include "llmjob.h"

void LLMJob::execute()
{
    // Parse error.json using rapidjson
    const char* filePath = "./Data/error.json";
    std::ifstream jsonFile(filePath);

    if (!jsonFile.is_open())
    {
        std::cerr << "Failed to open the JSON file: " << filePath << std::endl;
        return;
    }
    else
    {
        std::cout << "Successfully opened the JSON file: " << filePath << std::endl;
    }

    std::stringstream buffer;
    buffer << jsonFile.rdbuf();
    std::string jsonString = buffer.str();

    rapidjson::Document errorDoc;
    errorDoc.Parse(jsonString.c_str());

    // Use a stringstream to capture std::cout output
    std::ofstream coutCapture("./Data/llmprompt.txt");
    coutCapture << "My JobSystem is a custom build concurrency program that will try compiling a program, parse the error and eventually output the error message thown by the compiler. The error message outputs each individual errors and warnings, containing the file and the line and column in the file which the error occured in, the error/warning message and eventually the snippet of code the error occured in. I want you to examine each error, and give me a debug solution for each every single error and warning. First, give an explanation of why the file did not compile, and second give the fixed code in the context of the error. For each error/warning, I want you to ouput why the compiler error/warning exist, what is the fix, and finally the code snippet that corrects the code where the error is at. \n\nHere is the error message:\n";

    // Access elements in errorDoc as needed
    returnCode = errorDoc["return code"].GetInt();
    const rapidjson::Value& outputArray = errorDoc["output"];

    size_t count = 0;

    for (rapidjson::SizeType j = 0; j < outputArray.Size(); ++j)
    {
        const rapidjson::Value& fileObject = outputArray[j];
        // Access fileObject elements
        for (rapidjson::SizeType i = 0; i < fileObject.Size(); i++)
        {
            std::string file, line, column, message, messageType, snippet;
            if (fileObject[i].IsObject()) {
                file = fileObject[i]["file"].GetString();
                line = fileObject[i]["line"].GetString();
                column = fileObject[i]["column"].GetString();
                if(fileObject[i].HasMember("error"))
                {
                    message = fileObject[i]["error"].GetString();
                    messageType = "error";
                }
                else if(fileObject[i].HasMember("warning"))
                {
                    message = fileObject[i]["warning"].GetString();
                    messageType = "warning";
                }
                snippet = fileObject[i]["codeSnippet"].GetString();
                count++;
            }
            coutCapture << "Compiler " << messageType << " #" << count << " in file " << file << " at line " << line << " and column " << column << ". Compiler message prints: " << message << "The context of the code is: " << snippet << "\n\n";
        }
    }
    
    // Close the file
    jsonFile.close();

    // Use the stringstream to capture std::cout output
    coutCapture.close();

    // std::cout << "\n\nPrint llmPrompt\n\n" << llmPrompt << "\n\n\n\n";

    std::string command = "python3 ./Code/lib_job_system/LLMQuery.py ./Data/llmprompt.txt ./Data/LLMOutput.json";

    // std::cout << "\n\nPrint command\n\n" << command << "\n\npython3 Code/lib_job_system/LLMQuery.py Data/LLMOutput.json generate a python code that takes in 2 strings in an argument with sys.argv\n\n";

    // Open pipe to file

    FILE *pipe = popen(command.c_str(), "r");

    if (!pipe)
    {
        std::cout << "popen failed: failed to open pipe to file\n";
        return;
    }
}

void LLMJob::jobCompletedCallBack()
{
    // Process the LLM response and output
    std::cout << "LLM Job " << this->getUniqueID() << " has been executed \n";
    std::cout << "Compilation " << this->getUniqueID() << " return code: " << this->returnCode << "\n";
    std::cout << "LLM Job " << this->getUniqueID() << " output: " << this->output << "\n";
    std::cout << "LLM response is generated to Code/LLMOutput.json\n";
}

extern "C" Job* CreateLLMJob()
{
    return new LLMJob(0xffffffff, 1);
}