#ifndef SAVE_LOG_H
#define SAVE_LOG_H
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <ctime>
#include <unistd.h>
#include <iomanip>

void saveUserLog(tm *date_time, std::string username, std::string log_message);

#endif //SAVE_LOG_H