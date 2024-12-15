#include <iostream>
#include <string>
#include <array>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include "rapidjson/document.h"

#include "fsgenjob.h"

void FSGenJob::execute()
{
    std::string command = "python3 ./Code/lib_job_system/LLMQuery.py ./Data/fsgenprompt.txt ./Data/FSGen.json";

    // Open pipe to file

    FILE *pipe = popen(command.c_str(), "r");

    if (!pipe)
    {
        std::cout << "popen failed: failed to open pipe to file\n";
        return;
    }
    
    pclose(pipe);

    std::ifstream jsonFile("./Data/FSGen.json");

    if (!jsonFile.is_open())
    {
        std::cerr << "Failed to open the JSON file: ./Data/FSGen.json" << std::endl;
        return;
    }
    else
    {
        std::cout << "Successfully opened the JSON file: ./Data/FSGen.json" << std::endl;
    }

    std::stringstream buffer;
    buffer << jsonFile.rdbuf();
    std::string jsonString = buffer.str();

    rapidjson::Document document;
    document.Parse(jsonString.c_str());

    if (!document.IsObject()) {
        std::cerr << "Error parsing JSON." << std::endl;
    }

    const rapidjson::Value& choices = document["choices"];
    if (choices.IsArray() && choices.Size() > 0) {
        const rapidjson::Value& choice = choices[0];
        const char* text = choice["text"].GetString();

        // Open a file for writing
        std::ofstream outFile("./Data/flowscrpt.dot");

        // Check if the file is opened successfully
        if (!outFile.is_open()) {
            std::cerr << "Error opening file for writing." << std::endl;
        }
        std::string strtext(text);
        int pos = strtext.find("&");
//std::cout << "Pos: " << pos << std::endl;
//std::cout << "Length: " << strtext.length() << std::endl;
        strtext = strtext.substr(pos + 1, strtext.length() - 1);
//std::cout << "WOW: " << strtext << std::endl;
//std::cout << "WOW TEXT: " << text << std::endl;

        // Write the text into the file
        outFile << strtext;


        // Close the file
        outFile.close();

        std::cout << "Text written to ./Data/flowscrpt.dot successfully." << std::endl;
    } else {
        std::cerr << "No choices found in JSON." << std::endl;
    }


}

void FSGenJob::jobCompletedCallBack()
{
    // Process the LLM response and output
    std::cout << "FSGenJob " << this->getUniqueID() << " has been executed \n";
    // std::cout << "Compilation " << this->getUniqueID() << " return code: " << this->returnCode << "\n";
    // std::cout << "FSGenJob " << this->getUniqueID() << " output: " << this->output << "\n";
    std::cout << "FSGenJob response is generated to Code/LLMOutput.json\n";
}

extern "C" Job* CreateFSGenJob()
{
    return new FSGenJob(0xffffffff, 1);
}