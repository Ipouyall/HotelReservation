#include "saveLog.h"

void save_client_log(const date::year_month_day today_date, std::string path, bool success, std::string name,
                     std::string status_code, std::string status, std::string message, std::string cmd){
    struct stat st = {0};
    std::string direcory = path + name;
    if (stat(direcory.c_str(), &st) == -1) {
        mkdir(direcory.c_str(), 0700);
        std::ofstream outfile;
        std::string file_path = direcory + "/" + name + ".log";
        outfile.open(file_path, std::ios_base::app);
        outfile << "[date time]::[name]::[status-code]::[status]::[message]::[statement]" << std::endl;
    }
    std::ofstream outfile;
    std::string file_path = direcory + "/" + name + ".log";
    outfile.open(file_path, std::ios_base::app);

    auto tp  = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    if(success)
        outfile << "+";
    else
        outfile << "-";

    if(name == "general")
        name = "Unknown";

    
    outfile << "[" 
    << today_date << " " 
    << std::put_time(std::localtime(&tp), "%T" ) <<"]" << "::"
    << "[" << name << "]" << "::" << "[" << status_code << "]" << "::"
    << "[" << status << "]"
    << "[" << message << "]" << "::" << "[" << cmd << "]" << std::endl; 
}

inline bool file_exist (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

void save_server_log(const date::year_month_day today_date, std::string path, bool success, std::string username, 
                        int fd, std::string status, std::string status_code, std::string message, std::string kind){
    std::ofstream outfile;
    if(!file_exist(path + "server.log")){
        outfile.open(path + "server.log", std::ios_base::app);
        outfile << "[date time]::[username/client-fd]::[status-code]::[status]::[message]::[statement]" << std::endl;
    }
    else
        outfile.open(path + "server.log", std::ios_base::app);

    auto tp  = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::string name;

    if(username == "")
        name = std::to_string(fd);
    else
        name = username;

    if(success)
        outfile << "+";
    else
        outfile << "-";
    outfile << "[" 
    << today_date << " " 
    << std::put_time(std::localtime(&tp), "%T" ) <<"]" << "::"
    << "[" << name << "]" << "::" << "[" << status_code << "]" << "::"
    << "[" << status << "]"
    << "[" << message << "]" << "::" << "[" << kind << "]" << std::endl; 
}