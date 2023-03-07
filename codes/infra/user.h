#ifndef SRC_USER_H
#define SRC_USER_H
#include "filehandler.h"
#include <string>
#include <vector>

#define DEFAULT_USERS_PATH "./config/UsersInfo.json"

typedef struct UserData{
    int id;
    std::string username;
    std::string password;
    bool privilege; // user is admin
    int account_balance; // stores as string in json fila, why??
    std::string phone_number;
    std::string address;
} UserData;

std::vector<UserData> get_users_data(std::string path);

#endif //SRC_USER_H
