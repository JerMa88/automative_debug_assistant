#include <iostream>
#include <fstream>  // Include this header for file stream operations
#include <sstream>  // Include this header for string stream operations

#include "parsejob.h"
#include "outputjob.h"
#include "jobsystem.h"

#include <iostream>
#include <fstream>
#include <string>
#include "./rapidjson/document.h"
#include "./rapidjson/filewritestream.h"
#include "./rapidjson/stringbuffer.h"
#include "./rapidjson/prettywriter.h"

using namespace rapidjson;
using namespace std;



void ParseJob::execute()
{
    int linkerCommandCounter = 0;
    std::string output;
    std::string line;

    std::ifstream errorInput;
    errorInput.open("./Data/error_output_cj.txt");
    if(!errorInput.is_open())
        std::cerr << "error_output_cj.txt not open for read!\n";
    
    while (std::getline(errorInput, line))
    {
        output += line + '\n';
    }

    errorInput >> output;
    errorInput.close();

    // Parse the compiler output line by line
    std::istringstream compilerOutputStream(output);
    line = "";
    std::string oldFilePath = "";

    std::ofstream parseOutput;
    parseOutput.open("./Data/parse_output_pj.tsv");
    if(!parseOutput.is_open())
        std::cerr << "parse_output_pj.tsv not open for write!\n";

    while (std::getline(compilerOutputStream, line))
    {
        linkerCommandCounter++;
        int lineCounter = 0;
        if (line.find("error: linker command failed") != std::string::npos)
        {
            parseOutput << "nextObject" << "\t^";
            line = "";
            std::istringstream cos(output);
            std::stringstream buffer0;
            // std::cout << "lcc" << linkerCommandCounter << std::endl;
            while (std::getline(cos, line))
            {
                if (lineCounter < linkerCommandCounter && lineCounter > 0)
                    buffer0 << line << std::endl;
                lineCounter++;
            
            } //while (std::getline(cos, line) && !(line.find("error: linker command failed")));

            parseOutput << "linkerError" << "\t" << buffer0.str();
            break;
        }
        else if (line.find("error:") != std::string::npos || line.find("warning:") != std::string::npos)
        {
            parseOutput << "nextObject" << "\t^";
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos)
            {
                // Parse file path, line number, and column number
                std::string filePath = line.substr(0, colonPos);
                line = line.substr(colonPos + 1);
                size_t linePos = line.find(':');
                if (linePos != std::string::npos)
                {
                    std::string lineNumber = line.substr(0, linePos);
                    std::string columnNumber;
                    line = line.substr(linePos + 1);
                    linePos = line.find(':');
                    if (linePos != std::string::npos)
                    {
                        columnNumber = line.substr(0, linePos);
                        line = line.substr(linePos + 1);
                    }

                    parseOutput << "file" << "\t" << filePath << "^";
                    parseOutput << "line" << "\t" << lineNumber << "^";
                    parseOutput << "column" << "\t" << columnNumber << "^";
                    if (line.find("error:") != std::string::npos)
                        parseOutput << "error" << "\t" << line << "^";
                    else if(line.find("warning:") != std::string::npos)
                        parseOutput << "warning" << "\t" << line << "^";
                    
                    // write snippets from files
                    std::ifstream codeFile(filePath);
                    if (!codeFile.is_open())
                        std::cerr << "Failed to open the codeFile: " << filePath << "^";

                    std::string line;
                    int currentLineNumber = 0;
                    const int contextLines = 2;

                    // Read the file line by line until the desired line is reached

                    int lineCount = 0;
                    std::stringstream buffer1;
                    while (std::getline(codeFile, line) && lineCount < 5)
                    {
                        currentLineNumber++;
                        if ((stoi(lineNumber) - currentLineNumber) <= contextLines && (currentLineNumber - stoi(lineNumber)) <= contextLines)
                        { // You have found the desired line
                            buffer1 << line << std::endl;
                            lineCount++;
                        }
                    }
                    parseOutput << "codeSnippet" << "\t" << buffer1.str() << "^";

                    codeFile.close();

                    if (filePath == oldFilePath)
                    { // Add the error object to the same file
                        //do nothing
                    }
                    else
                    {                                                       // pushback the error object into a new file
                        parseOutput << "nextFile" << "\t^";

                        oldFilePath = filePath;
                    }
                }
            }
        }
    }
    parseOutput << "nextObject" << "\t^";
    parseOutput.close();

    std::cout << "Parse Job " << this->getUniqueID() << " has been executed\n";
    
    OutputJob* oj = new OutputJob(0xffffffff, 1);
    JobSystem::createOrGet()->queueJob(oj);
    // call OutputJob in ParseJob OnJobComplete AS SOON AS PJ EXECUTES: Hard Code Dependency
}

void ParseJob::jobCompletedCallBack()
{
}

extern "C" Job* CreateParseJob()
{
    return new ParseJob(0xffffffff, 1);
}