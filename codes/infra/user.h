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

    std::string to_string();
} UserData;

std::vector<UserData> read_users_data(std::string path);
std::string generate_token();

enum class UserRole{ ADMIN, USER };

class UserManager{
private:
    std::vector<UserData> users;

private:
    bool id_exists(int id);
    int create_Id();
    int search_by_token(std::string token);
    int search_by_username(std::string username);
    
public:
    UserManager();
    bool username_exist(std::string username);
    bool user_validation(std::string username, std::string password);
    bool signup(std::string username, std::string password,
                         int balance, std::string phone, std::string addr);
    std::string login(std::string username, int ufd);
    bool logout(std::string token);
    bool is_logged_in(std::string username);
    UserRole get_role(std::string token);
    void printINfo();
    bool reduce_balance(std::string token, int price);
    bool increase_balance(std::string token, int amount);
    bool edit_information(std::string token, std::string new_pass, std::string phone, std::string addr);
    int get_id(std::string token);
    bool have_enough_money(std::string token, int price);
    void save(std::string path);
    void client_dead(int fd);
    std::string get_user_data(std::string token);
    std::string get_users_data(std::string token);
};




#endif //SRC_USER_H
