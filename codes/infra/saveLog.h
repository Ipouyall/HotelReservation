#ifndef SAVE_LOG_H
#define SAVE_LOG_H
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <ctime>
#include <unistd.h>
#include <iomanip>
#include <date/date.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <iostream>
using json = nlohmann::json;

void save_client_log(const date::year_month_day today_date, std::string path, bool success, std::string name, 
                        std::string status_code, std::string status, std::string message, std::string cmd);

void save_server_log(const date::year_month_day today_date, std::string path, bool success, std::string username, 
                        int fd, std::string status_code, std::string status, std::string message, std::string kind);

#endif //SAVE_LOG_H