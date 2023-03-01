#include "filehandler.h"
#include <iostream>
#include <glog/logging.h>

using json = nlohmann::json;

json readJsonFile(std::string filePath)
{
    // Read file contents into a string
    std::ifstream fileStream(filePath);
    if (!fileStream.is_open()) {
        LOG(FATAL) << "Failed to open JSON file: [" << filePath << ']';
        exit(1);
    }
    std::string fileContent((std::istreambuf_iterator<char>(fileStream)),
                            std::istreambuf_iterator<char>());

    // Parse the JSON string
    json jsonData = json::parse(fileContent);

    return jsonData;
}
