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

json server_response(std::string kind, std::string status_code, std::string msg){
    // TODO: read error file and set status from that; also add server class to initialize data
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
    json response;
    if (!um.user_validation(username, password)) 
        response = server_response(
            "error", "430", "Invalid username or password.", "Sign_in failed due to invalid pass os uname."
        );
    else {
        std::string token = um.login(username, fd);
        response = server_response(
                "success", "230", "You logged in successfully"
        );
        response["token"] = token;
    }
    return response.dump();
}

std::string command::signup(json& j, UserManager& um) {
    // TODO: client, after validating username is free, should again send uname,
    //      we have to store username and inject in here
    //      and check if user exists(error => 451) o.w. (311)

    std::string username = j["username"];
    std::string pass = j["password"];
    int ba = j["balance"];
    std::string pn = j["phone"];
    std::string addr = j["addr"];
    json response;

    auto succeeded = um.signup(username, pass, ba, pn, addr);
    if(succeeded)
        response = server_response("success", "")

}

std::string command::logout(json &j_in, UserManager &um) {
    std::string token = j_in["token"];
    auto succeeded = um.logout(token);
    // TODO: 201 on success
}