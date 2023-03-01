#include "filehandler.h"
#include <iostream>

using json = nlohmann::json;

json readJsonFile(std::string filePath)
{
    // Read file contents into a string
    std::ifstream fileStream(filePath);
    if (!fileStream.is_open()) {
        std::cerr << "Failed to open JSON file: [" << filePath << ']' << std::endl;
        exit(1);
    }
    std::string fileContent((std::istreambuf_iterator<char>(fileStream)),
                            std::istreambuf_iterator<char>());

    // Parse the JSON string
    json jsonData = json::parse(fileContent);

    return jsonData;
}
