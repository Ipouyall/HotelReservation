#include "saveLog.h"

void saveUserLog(tm *date_time, std::string username, std::string log_message){
    struct stat st = {0};
    std::string direcory = "./" + username;
    if (stat(direcory.c_str(), &st) == -1) {
        mkdir(direcory.c_str(), 0700);
    }
    std::ofstream outfile;
    std::string file_path = direcory + "/" + username + ".log";
    outfile.open(file_path, std::ios_base::app);

    time_t now = time(0);
    tm *ltm = localtime(&now);
    outfile.fill('0');
    date_time->tm_sec = ltm->tm_sec;
    date_time->tm_min = ltm->tm_min;
    date_time->tm_hour = ltm->tm_hour;
    outfile << "time: " 
    << std::setw(2) << date_time->tm_mday << "-" 
    << std::setw(2) << date_time->tm_mon << "-"
    << date_time->tm_year<< " " 
    << std::setw(2) << date_time->tm_hour << ":" 
    << std::setw(2) << date_time->tm_min << ":" 
    << std::setw(2) << date_time->tm_sec << std::endl;
    outfile << log_message << std::endl;
}