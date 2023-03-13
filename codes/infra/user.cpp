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

int UserManager::search_by_username(std::string username){
    for(int i = 0;i < users.size();i++)
        if(users[i].username == username)
            return i;
    return -1;
}

int UserManager::search_by_token(std::string token){
    for(int i = 0;i < users.size();i++)
        if(users[i].token == token)
            return i;
    return -1;
}

bool UserManager::username_exist(std::string username){
    for(int i = 0;i < users.size();i++)
        if(username == users[i].username)
            return true;
    return false;
}

bool UserManager::user_validation(std::string username, std::string password){
    for(int i = 0;i < users.size();i++)
        if(username == users[i].username)
            return users[i].password == password && (!users[i].is_logged_in);
    return false;
}

bool UserManager::id_exists(int id){
    for(int i = 0;i < users.size();i++){
        if(id == users[i].id)
            return true;
    }
    return false;
}

int UserManager::create_Id(){
    int id = 0;
    while(true){
        if(!id_exists(id))
            return id;
        id++;
    }
}

bool UserManager::signup(std::string username, std::string password,
                         int balance, std::string phone, std::string addr){
    if(username == "" || password == "" || addr == "")
        return false;
    // valid phones:
    // 09123456789, +989123456789
    if(phone.size() != 11 && phone.size() != 13)
        return false;
    if(username_exist(username))
        return false;
    UserData new_user;
    new_user.id = create_Id();
    new_user.username = username;
    new_user.password = password;
    new_user.is_logged_in = false;
    new_user.privilege = false;
    new_user.phone_number = phone;
    new_user.address = addr;
    new_user.account_balance = balance;
    new_user.socket_fd = -1;
    new_user.token = "";

    users.push_back(new_user);
    return true;
}

std::string UserManager::login(std::string username, int ufd) {
    int index = search_by_username(username);
    if (index == -1)
        return "";
    users[index].is_logged_in = true;
    users[index].token = generate_token();
    users[index].socket_fd = ufd;
    return users[index].token;
}

bool UserManager::logout(std::string token){
    int index = search_by_token(token);
    if(index != -1 && users[index].is_logged_in){
        users[index].is_logged_in = false;
        users[index].token = "";
        return true;
    }
    return false;
}

UserRole UserManager::get_role(std::string token){
    int index = search_by_token(token);
    
    return users[index].privilege ? UserRole::ADMIN : UserRole::USER;
}

bool UserManager::is_logged_in(std::string username){
    int index = search_by_username(username);
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

bool UserManager::reduce_balance(std::string token, int price){
    int index = search_by_token(token);
    if(index != -1 && users[index].account_balance >= price){
        users[index].account_balance -= price;
        return true;
    }
    return false;
}

bool UserManager::increase_balance(std::string token, int amount){
    int index = search_by_token(token);
    if(index != -1){
        users[index].account_balance += amount;
        return true;
    }
    return false;
}

bool UserManager::edit_information(std::string token, std::string new_pass, 
                                    std::string phone="", std::string addr=""){
    if(new_pass == "")
        return false;
    int index = search_by_token(token);
    if(index != -1){
        users[index].password = new_pass;
        if(users[index].privilege) 
            return true;
        users[index].address = addr;
        users[index].phone_number = phone;
        return true;
    }
    return false;
}

int UserManager::get_id(std::string token){
    int index = search_by_token(token);
    if(index != -1){
        return users[index].id;
    }
    return -1;
}

void UserManager::client_dead(int fd) {
    for(int i=0; i < users.size(); i++)
        if(fd == users[i].socket_fd && users[i].is_logged_in){
            users[i].token="";
            users[i].socket_fd=-1;
            users[i].is_logged_in=false;
            LOG(INFO) << "Client dead (" << users[i].username << ")";
            return;
        }
}