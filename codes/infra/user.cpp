#include <openssl/rand.h>
#include <openssl/sha.h>
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

std::string generate_token(){
    unsigned char rand_bytes[32];
    RAND_bytes(rand_bytes, sizeof(rand_bytes));

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(rand_bytes, sizeof(rand_bytes), hash);

    std::string token;
    token.reserve(2 * SHA256_DIGEST_LENGTH);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        token += "0123456789abcdef"[hash[i] >> 4];
        token += "0123456789abcdef"[hash[i] & 0xf];
    }
    return token;
}
