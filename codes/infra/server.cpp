#include "server.h"
#include <iostream>
#include <glog/logging.h>

serverConfig get_server_config(std::string path) {
    LOG(INFO) << "Start initializing server configuration from: [" << path << ']';
    auto conf = serverConfig{};

    auto j = readJsonFile(path);
    if (j == nullptr)
        return conf;

    conf.host_name = j["hostName"];
    conf.port = j["commandChannelPort"];

    return conf;
}

std::string command::diagnose(std::string command) {
    json json_data_in = json::parse(command);
    // TODO: parse command regarding 'kind'

}

std::string command::sign_in(json& j_in, UserManager& um, int fd){
    std::string username = j_in["username"];
    std::string password = j_in["password"];
    if (!um.user_validation(username, password)) {
        // TODO: signup failed, error => 430
    }
    std::string token = um.login(username, fd);
    // TODO: on success, return 230 and token
}

std::string command::signup(json& j, UserManager& um) {
    // TODO: in prev stage before calling this,
    //      we have to store username and inject in here
    //      and check if user exists(error => 451) o.w. (311)

    std::string username = j["username"];
    std::string pass = j["password"];
    int ba = j["balance"];
    std::string pn = j["phone"];
    std::string addr = j["addr"];

    auto succeeded = um.signup(username, pass, ba, pn, addr);
    // TODO:  503 on error and 231 for success
}