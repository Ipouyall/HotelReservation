#ifndef SRC_SERVER_H
#define SRC_SERVER_H
#include <nlohmann/json.hpp>
#include "filehandler.h"
#include "user.h"
#include <string>

using json = nlohmann::json;

#define DEFAULT_SERVER_PATH "./config/config.json"
#define DEFAULT_ERRORS_PATH "./config/Errors.json"

typedef struct serverConfig {
    std::string host_name;
    int port;
} serverConfig;

serverConfig get_server_config(std::string path);


class Server{
private:
    serverConfig config;
    json errors;
    int fd;

public:
    Server();
    int get_fd();
    std::string diagnose(std::string command, UserManager& um, int client_fd);

private:
    json response(std::string kind, std::string status_code, std::string msg);
    std::string sign_in(json& j_in, UserManager& um, int fd);
    std::string signup(json& j_in, UserManager& um);
    std::string is_uname_available(json& j_in, UserManager& um);
    std::string logout(json& j_in, UserManager& um);
    std::string view_user_information(json& j_in, UserManager& um);
    std::string view_all_users(json& j_in, UserManager& um); // TODO: implement dependency
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
};

#endif //SRC_SERVER_H
