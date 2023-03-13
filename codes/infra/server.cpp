#include "server.h"
#include "socketUtils.h"
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

Server::Server() {
    config = get_server_config(DEFAULT_SERVER_PATH);
    errors = readJsonFile(DEFAULT_ERRORS_PATH);
    fd = setupServer(config.host_name.c_str(), config.port);
}

int Server::get_fd() { return fd;}

std::string Server::diagnose(std::string command, UserManager& um, int client_fd) {
    LOG(INFO) << "Diagnosing incoming request";
    json json_data_in = json::parse(command);
    // TODO: update arguments of this function
    std::string cmd = json_data_in["kind"];
    std:: string rsp;
    if(cmd == "sign_in")
        rsp = sign_in(json_data_in, um, client_fd);
    else if(cmd == "free_username")
        rsp = is_uname_available(json_data_in, um);
    else if(cmd == "sign_up")
        rsp = signup(json_data_in, um);
    else if(cmd == "logout")
        rsp = logout(json_data_in, um);
    return rsp;
}

json Server::response(std::string kind, std::string status_code, std::string msg){
    std::string status_msg = errors[status_code];
    json rsp;
    rsp["kind"] = kind;
    rsp["status_code"] = status_code;
    rsp["status"] = status_msg;
    rsp["message"] = msg;
    return rsp;
}

std::string Server::sign_in(json& j_in, UserManager& um, int fd){
    LOG(INFO) << "Login request received on (" << fd << ")";
    std::string username = j_in["username"];
    std::string password = j_in["password"];
    json rsp;
    if (!um.user_validation(username, password))
    {
        rsp = response(
                "error", "430", "Sign_in failed due to invalid password/username or you logged in before."
        );
        LOG(WARNING) << "Login request from (" << fd << ") failed!";
    }
    else
    {
        std::string token = um.login(username, fd);
        rsp =  response(
                "success", "230", "You logged in successfully"
        );
        rsp["token"] = token;
        LOG(INFO) << "Login request from (" << fd << ") succeeded.";
    }
    return rsp.dump();
}

std::string Server::is_uname_available(json &j_in, UserManager &um) {
    LOG(INFO) << "Checking username is available";
    std::string username = j_in["username"];
    bool available = !um.username_exist(username);
    json rsp;

    if (available)
    {
        rsp = response("success", "000", "Just validated you username, please complete your registration");
        LOG(INFO) << "Username is available";
    }
    else
    {
        rsp = response("error", "451", "Username exists, try another one.");
        LOG(WARNING) << "Username is available";
    }
    return rsp.dump();
}

std::string Server::signup(json& j, UserManager& um) {
    // TODO: client, after validating username is free, should again send uname,
    LOG(INFO) << "Signup request received";
    std::string username = j["username"];
    std::string pass = j["password"];
    int ba = j["balance"];
    std::string pn = j["phone"];
    std::string addr = j["addr"];
    json rsp;

    auto succeeded = um.signup(username, pass, ba, pn, addr);
    if(succeeded)
    {
        LOG(INFO) << "User signed up successfully";
        rsp = response("success", "311", "We honor to announce you are part of our community from now on.");
    }
    else
    {
        LOG(WARNING) << "User signup failed";
        rsp = response("error", "451", "Signing up failed, make sure you entered valid data.");
    }
    return rsp.dump();
}

std::string Server::logout(json &j_in, UserManager &um) {
    // TODO: client fd should be cleared from fd-set
    LOG(INFO) << "Logout request received";
    std::string token = j_in["token"];
    auto succeeded = um.logout(token);
    json rsp;

    rsp = response("success", "201", "Hope to see you later!");
    return rsp.dump();
}
