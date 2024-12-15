#include <iostream>
#include <string>
#include <array>
#include <unistd.h> // Include this header for getcwd()
#include <fstream>  // Include this header for file stream operations
#include <sstream>  // Include this header for string stream operations
#include <iomanip>  // Include this header for manipulating file names

#include "compilejob.h"
#include "parsejob.h"
#include "jobsystem.h"

void CompileJob::execute()
{
    // char b[1024]; // DEBUG: This buffer will hold the current working directory

    // if (getcwd(b, sizeof(b)) != NULL) {
    //     std::cout << "Current working directory: " << b << std::endl;
    // } else {
    //     perror("getcwd");
    // }
    // std::cout << "\n\n\n!!!For makefile, make sure current working directory is /Users/zma/Documents/GitHub/lab-1-multithreading-JerMa88, not in /build!!!\n\n\n";

    std::array<char, 128> buffer;
    // std::string command = "clang++ -g -std=c++14 ./Code/compilecode/*.cpp -o output";
    std::string command = "make automated";

    // Redirect cerr to cout
    command.append(" 2>&1");

    // Open pipe to file
    FILE *pipe = popen(command.c_str(), "r");

    if (!pipe)
    {
        std::cout << "popen failed: failed to open pipe to file\n";
        return;
    }

    while (fgets(buffer.data(), 128, pipe) != NULL)
    {
        this->output.append(buffer.data());
    }

    // close pipe and get return code
    this->returnCode = pclose(pipe);

    std::ofstream errorOutput;
    errorOutput.open("./Data/error_output_cj.txt");
    if(!errorOutput.is_open())
        std::cerr << "error_output_cj.txt not open for write!\n";
    errorOutput << output;
    errorOutput.close();
    
    std::ofstream returnCode;
    returnCode.open("./Data/return_code.txt");
    if(!returnCode.is_open())
        std::cerr << "return_code.txt not open for write!\n";
    returnCode << this->returnCode;
    returnCode.close();
    
    std::cout << "Compile Job " << this->getUniqueID() << " has been executed \n";
}

void CompileJob::jobCompletedCallBack()
{
    if(this->returnCode == 0)
    {
        std::cout << "Compile Complete with no errors!\n";
        std::cout << "Compile Job " << this->getUniqueID() << " return code: " << this->returnCode << "\n";
        std::cout << "Compile Job " << this->getUniqueID() << " output: " << this->output << "\n";
        std::cout << "To compile again with Compiler Job System, delete \"automated\" in this code space.";
    }
    else
    {
        std::cout << "Compile Failed !\n";
        std::cout << "Errors and warnings has been written to ../Data/error.json.\n";
    }
} //cout called in the main thread

extern "C" Job* CreateCompileJob()
{
    return new CompileJob(0xffffffff, 1);
}