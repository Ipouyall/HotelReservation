#include <array>
#include <glog/logging.h>
#include "socketUtils.h"


int acceptClient(int server_fd) {
    LOG(INFO) << "New client reached!";
    int client_fd;
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);
    client_fd = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t*) &address_len);
    LOG(INFO) << "Accepted client with fd::" << client_fd;
    return client_fd;
}
std::string get_program_name_on_port(int port) {
    std::string command = "lsof -i :" + std::to_string(port) + "| awk '!/COMMAND/{print $1}'";
    std::array<char, 128> buffer;
    std::string result = "";
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    if (!result.empty() && result.back() == '\n') {
        result.pop_back(); // remove newline character if it exists
    }
    return result;
}

int setupServer(const char* server_ip, int port, int listen_for) {
    LOG(INFO) << "Starting server...";
    struct sockaddr_in address;
    const int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    const int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(server_ip);
    address.sin_port = htons(port);
    int result=bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    if(result==-1) {
        char err_msg[512];
        snprintf(err_msg, sizeof(err_msg),
                 "Another server from %s is already listening on %s:%d",
                 get_program_name_on_port(port).c_str(), server_ip, port);
        LOG(FATAL) << err_msg;
    }
    listen(server_fd, listen_for);
    LOG(INFO) << "Server Listening on: " << server_ip << ":" << port << " for " << listen_for << "client";
    return server_fd;
}

int connectServer(const char* server_ip, int server_port, bool& succeeded) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    LOG(INFO) << "Connecting to server: " << server_ip << ":" << server_port;
    if (sockfd == -1) {
        perror("socket creation >>> ");
        LOG(FATAL) << "Socket failed";
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = inet_addr(server_ip);

    if (connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        succeeded = false;
        LOG(ERROR) << "Connection refused";
        perror("--connect:");
    }
    else {
        LOG(INFO) << "Connected successfully";
        succeeded = true;
    }
    return sockfd;
}

bool reconnectServer(const char* server_ip, int server_port, int& sockfd){
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    LOG(INFO) << "Reconnecting to server: " << server_ip << ":" << server_port;
    if (sockfd == -1) {
        perror("socket creation >>> ");
        LOG(FATAL) << "Socket failed";
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = inet_addr(server_ip);

    if (connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        LOG(ERROR) << "Connection refused:";
        perror("--connect:");
        return false;
    }
    LOG(INFO) << "Connected successfully";
    return true;
}