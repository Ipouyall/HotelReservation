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
