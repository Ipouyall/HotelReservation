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
    std::string sign_in(json& j_in, UserManager& um, int fd);
    std::string signup(json& j_in, UserManager& um);
}

#endif //SRC_SERVER_H
