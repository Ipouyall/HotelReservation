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
    int account_balance; // stores as string in json file, why??
    std::string phone_number;
    std::string address;
    bool is_logged_in = false;
    int socket_fd = -1;
    std::string token = "";
} UserData;

std::vector<UserData> get_users_data(std::string path);
std::string generate_token();

enum class UserRole{ ADMIN, USER };


#endif //SRC_USER_H
