#ifndef SRC_FILEHANDLER_H
#define SRC_FILEHANDLER_H

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

json readJsonFile(std::string filePath);
void writeJsonFile(std::string filePath, nlohmann::ordered_json content);

#endif //SRC_FILEHANDLER_H
