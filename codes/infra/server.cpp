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

std::string command::diagnose(std::string command, UserManager& um, int client_fd) {
    json json_data_in = json::parse(command);
    // TODO: update arguments of this function
    std::string command = json_data_in["kind"];
    std:: string response;
    switch(command){
        case("sign_in"):
        response = sign_in(json_data_in, um, client_fd);
        break;
    } 

}

json server_response(std::string kind, std::string status_code, std::string status, std::string token, std::string msg){
    json rsp;
    rsp["kind"] = kind;
    rsp["status_code"] = status_code;
    rsp["status"] = status;
    rsp["message"] = msg;
    return rsp;
}

std::string command::sign_in(json& j_in, UserManager& um, int fd){
    std::string username = j_in["username"];
    std::string password = j_in["password"];
    if (!um.user_validation(username, password)) 
        return server_response(
            "error", "430", "Invalid username or password.", "", "Sign_in failed due to invalid pass os uname."
        );
    std::string token = um.login(username, fd);
    return server_response(
        "success", "230", "User logged in.", token, "You logged in successfully"
    );
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

std::string command::logout(json &j_in, UserManager &um) {
    std::string token = j_in["token"];
    auto succeeded = um.logout(token);
    // TODO: 201 on success
}