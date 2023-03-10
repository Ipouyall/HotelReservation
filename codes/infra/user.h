#ifndef SRC_USER_H
#define SRC_USER_H
#include "filehandler.h"
#include <string>
#include <vector>

#define DEFAULT_USERS_PATH "./config/UsersInfo.json"
#define SHA256_DIGEST_LENGTH 32

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

class UserManager{
private:
    std::vector<UserData> users;

private:
    bool idExist(int id);
    int createId();
    int searchByToken(std::string token);
    int searchByUsername(std::string username);
public:
    UserManager();
    void setFd(std::string token, int fd);
    bool usernameExist(std::string username);
    bool userValidation(std::string username, std::string password);
    bool addUser(std::string username, std::string password,
                         int balance, std::string phone, std::string addr);
    void setToken(std::string username);
    std::string userLoggedIn(std::string username);
    bool userLoggedOut(std::string token);
    bool isLoggedIn(std::string username);
    UserRole getRole(std::string token);
    void printINfo();
    bool reduceBalance(std::string token, int price);
    bool editInformation(std::string token, std::string new_pass, std::string phone, std::string addr);
    // TODO: add functionality get user/users data
};




#endif //SRC_USER_H
