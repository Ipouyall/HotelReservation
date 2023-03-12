#ifndef SRC_SERVER_H
#define SRC_SERVER_H
#include <nlohmann/json.hpp>
#include "filehandler.h"
#include "user.h"
#include <string>

using json = nlohmann::json;

#define DEFAULT_SERVER_PATH "./config/config.json"

typedef struct serverConfig {
    std::string host_name;
    int port;
} serverConfig;

serverConfig get_server_config(std::string path);

namespace command {
    std::string diagnose(std::string command);
    std::string sign_in(json& j_in, UserManager& um, int fd
    std::string signup(json& j_in, UserManager& um);
    std::string logout(json& j_in, UserManager& um);
    std::string view_user_information(); // TODO: implement dependency
    std::string view_all_users(); // TODO: implement dependency
    std::string view_rooms_info(); // TODO: implement dependency
    std::string book_a_room(); // TODO: implement dependency
    std::string cancel_a_room(); // TODO: implement dependency
    std::string pass_days(); // TODO: implement dependency
    std::string edit_user_info(); // TODO: implement dependency
    std::string leave_room(); // TODO: implement dependency
    std::string empty_room(); // TODO: implement dependency
    std::string add_a_room(); // TODO: implement dependency
    std::string modify_a_room(); // TODO: implement dependency
    std::string remove_a_room(); // TODO: implement dependency
}

#endif //SRC_SERVER_H
