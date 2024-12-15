#include <iostream>
#include <fstream>
#include <string>
#include "./rapidjson/document.h"
#include "./rapidjson/filewritestream.h"
#include "./rapidjson/stringbuffer.h"
#include "./rapidjson/prettywriter.h"

#include "outputjob.h"

using namespace rapidjson;
using namespace std;

void OutputJob::execute()
{
    const char* inputFilePath = "./Data/parse_output_pj.tsv";
    const char* outputFilePath = "./Data/error.json";

    ifstream tsvFile(inputFilePath);
    Document jsonWriter;
    jsonWriter.SetObject();

    Value filesArray(kArrayType);
    Value errorObject(kObjectType);
    Value linkerErrorObject(kObjectType);

    string line;
    while (getline(tsvFile, line, '^')) {
        // cout << "line: " << line << "\n";
        size_t delimiterPos = line.find('\t');
        if (delimiterPos == string::npos) {
            cerr << "Invalid TSV format: " << line << endl;
            continue;
        }

        string name = line.substr(0, delimiterPos);
        string value = line.substr(delimiterPos + 1);
        if (name == "nextFile") {
            Value fileObject(kArrayType); // Inner array to hold objects
            filesArray.PushBack(fileObject, jsonWriter.GetAllocator());
        } else if (name == "nextObject") {
            if (!errorObject.ObjectEmpty()) {
                filesArray[filesArray.Size() - 1].PushBack(errorObject, jsonWriter.GetAllocator());
                errorObject = Value(kObjectType);  // Create a new object
            }
        } else if (name == "linkerError") {
            Value nameValue(name.c_str(), jsonWriter.GetAllocator());
            Value valueValue(value.c_str(), jsonWriter.GetAllocator());
            linkerErrorObject.AddMember(nameValue, valueValue, jsonWriter.GetAllocator());
            filesArray.PushBack(linkerErrorObject, jsonWriter.GetAllocator());
            break;
        } else {
            Value nameValue(name.c_str(), jsonWriter.GetAllocator());
            Value valueValue(value.c_str(), jsonWriter.GetAllocator());
            errorObject.AddMember(nameValue, valueValue, jsonWriter.GetAllocator());
        }
    }


    ifstream errorCode("./Data/return_code.txt");
    int returnCode = 0xffff;

    errorCode >> returnCode;
    errorCode.close();

    jsonWriter.AddMember("return code", returnCode, jsonWriter.GetAllocator());
    jsonWriter.AddMember("output", filesArray, jsonWriter.GetAllocator());

    // Open the output file
    FILE *jsonFile = fopen(outputFilePath, "w");
    if (!jsonFile)
    {
        std::cout << "fopen failed: failed to open jsonFile\n";
    }

    // Create a buffer for writing
    char buffer[65536]; // Adjust the buffer size as needed

    // Write the JSON data to the jsonFile
    rapidjson::FileWriteStream os(jsonFile, buffer, sizeof(buffer));
    // rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
    rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);

    jsonWriter.Accept(writer);
    fclose(jsonFile);

    std::cout << "Output Job " << this->getUniqueID() << " has been executed\n";
}

void OutputJob::jobCompletedCallBack()
{
    std::cout << "../Data/error.json finished writing.\n";
}

extern "C" Job* CreateOutputJob()
{
    return new OutputJob(0xffffffff, 1);
}