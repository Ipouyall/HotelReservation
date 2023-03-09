#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <glog/logging.h>
#include "user.h"


std::vector<UserData> get_users_data(std::string path){
    LOG(INFO) << "Start initializing users data from: [" << path << ']';
    std::vector<UserData> users_data;

    auto jd = readJsonFile(path);
    if (jd == nullptr)
        return users_data;

    for (auto user : jd["users"]){
        UserData user_data;
        user_data.id = user["id"];
        user_data.username = user["user"];
        user_data.password = user["password"];
        user_data.privilege = user["admin"];
        user_data.is_logged_in = false;
        if (!user_data.privilege){
            user_data.account_balance = std::stoi(user["purse"].get<std::string>());
            user_data.phone_number = user["phoneNumber"];
            user_data.address = user["address"];
        }
        users_data.push_back(user_data);
    }
    return users_data;
}

std::string generate_token() {
    // Create a random number generator with a random seed
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    // Generate a random token
    std::stringstream ss;
    for (int i = 0; i < 32; i++) {
        int nibble = dis(gen);
        ss << std::hex << nibble;
    }
    std::string token = ss.str();

    return token;
}

UserManager::UserManager(){
    users = get_users_data(DEFAULT_USERS_PATH);
}
int UserManager::searchByUsername(std::string username){
    for(int i = 0;i < users.size();i++){
        if(users[i].username == username)
            return i;
    }
    return -1;
}
int UserManager::searchByToken(std::string token){
    for(int i = 0;i < users.size();i++){
        if(users[i].token == token)
            return i;
    }
    return -1;
}

void UserManager::setFd(std::string token, int fd){
    int index = searchByToken(token);
    if(index != -1){
        users[index].socket_fd = fd;
    }
}

bool UserManager::usernameExist(std::string username){
    for(int i = 0;i < users.size();i++){
        if(username == users[i].username)
            return true;
    }
    return false;
}

bool UserManager::userValidation(std::string username, std::string password){
    for(int i = 0;i < users.size();i++){
        if(username == users[i].username){
            if(users[i].password == password)
                return true;
            else
                return false;
        }
    }
    return false;
}
bool UserManager::idExist(int id){
    for(int i = 0;i < users.size();i++){
        if(id == users[i].id)
            return true;
    }
    return false;
}

int UserManager::createId(){
    int id = 0;
    while(true){
        if(!idExist(id))
            return id;
        id++;
    }
}

std::string UserManager::addUser(std::string username, std::string password,
                         int balance, std::string phone, std::string addr){
    if(usernameExist(username))
        return "";
    UserData new_user;
    new_user.id = createId();
    new_user.username = username;
    new_user.password = password;
    new_user.is_logged_in = false;
    new_user.privilege = false;
    new_user.phone_number = phone;
    new_user.address = addr;
    new_user.account_balance = balance;
    new_user.socket_fd = -1;
    new_user.token = generate_token();

    users.push_back(new_user);
    return new_user.token;
}

void UserManager::setToken(std::string username){
    int index = searchByUsername(username);
    if(index != -1)
        users[index].token = generate_token();
}

std::string UserManager::userLoggedIn(std::string username){
    int index = searchByUsername(username);
    if(index != -1){
        users[index].is_logged_in = true;
        users[index].token = generate_token();
        return users[index].token;
    }
    return "";
}

bool UserManager::userLoggedOut(std::string token){
    int index = searchByToken(token);
    if(index != -1){
        users[index].is_logged_in = false;
        return true;
    }
    return false;
}

UserRole UserManager::getRole(std::string token){
    int index = searchByToken(token);
    
    if(users[index].privilege)
        return UserRole::ADMIN;
    else
        return UserRole::USER;
}

bool UserManager::isLoggedIn(std::string token){
    int index = searchByToken(token);
    if(index != -1 && users[index].is_logged_in)
        return true;
    return false;
}

void UserManager::printINfo(){
    for(int i = 0;i < users.size();i++){
        std::cout << "_____________User " << i <<"_____________" << std::endl;
        std::cout << "id = " << users[i].id << std::endl;
        std::cout << "username = " << users[i].username << std::endl;
        std::cout << "password = " << users[i].password << std::endl;
        std::cout << "address = " << users[i].address << std::endl;
        std::cout << "account_balance = " << users[i].account_balance << std::endl;
        std::cout << "admin = " << users[i].privilege << std::endl;
        std::cout << "token = " << users[i].token << std::endl;
    }
}

bool UserManager::reduceBalance(std::string token, int price){
    int index = searchByToken(token);
    if(index != -1 && users[index].account_balance >= price){
        users[index].account_balance -= price;
        return true;
    }
    return false;
}
