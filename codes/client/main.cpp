#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <glog/logging.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "../infra/socketUtils.h"
#include "../infra/server.h"

#define BUFFER_SIZE 1024

using namespace std;

int connect_to_server(serverConfig& server_info, int max_attempts=10) {
    int sockfd, attempt=1;

    bool connected;
    sockfd = connectServer(server_info.host_name.c_str(), server_info.port, connected);

    while (!connected && attempt < max_attempts){
        attempt++;
        LOG(WARNING) << "Reconnecting in 2 seconds";
        sleep(2);
        connected = reconnectServer(server_info.host_name.c_str(), server_info.port, sockfd);
    }
    if(!connected){
        LOG(FATAL) << "Couldn't connect to server";
    }
    return sockfd;
}

void signalHandler(int signum) {
    LOG(WARNING) << "Ending client...";
    google::ShutdownGoogleLogging();
    exit(0);
}

int main(int argc, char const *argv[]) {
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();
    FLAGS_colorlogtostderr = true;
    FLAGS_log_prefix = true;
    FLAGS_logtostderr = true;
    FLAGS_alsologtostderr = true;
    LOG(INFO) << "Initializing Client...";
    signal(SIGINT, signalHandler);

    auto server_info = get_server_config(DEFAULT_SERVER_PATH);
    int sockfd = connect_to_server(server_info);

    char buffer[BUFFER_SIZE];

    fd_set master_set, working_set;
    FD_ZERO(&master_set);
    FD_SET(sockfd, &master_set);
    FD_SET(STDIN_FILENO, &master_set);

    while (true) {
        working_set = master_set;
        select(FD_SETSIZE, &working_set, NULL, NULL, NULL);

        for (int i = 0; i < FD_SETSIZE; i++) {
            if (FD_ISSET(i, &working_set)) {
                if (i == STDIN_FILENO) { // input from stdin
                    memset(buffer, 0, BUFFER_SIZE);
                    read(STDIN_FILENO, buffer, BUFFER_SIZE);
                    buffer[strlen(buffer)-1] = '\0';

                    if(send(sockfd, buffer, strlen(buffer), 0) != -1)
                        LOG(INFO) << "Your response sent to server.";
                    else
                        LOG(ERROR) << "Error on sending your message to the server!";
                }
                else if (i == sockfd) { // sth from server is reached
                    memset(buffer, 0, BUFFER_SIZE);
                    int bytes_received = recv(sockfd , buffer, BUFFER_SIZE, 0);
                    if (bytes_received == 0) { // recovering server when server is down
                        close(sockfd);
                        FD_CLR(sockfd, &master_set);
                        LOG(WARNING) << "Server is Down";
                        sockfd = connect_to_server(server_info);
                        FD_SET(sockfd, &master_set);
                        continue;
                    }
                    LOG(INFO) << "Server response: " << buffer;
                }
                else {
                    LOG(WARNING) << "Unknown file descriptor: " << i ;
                }
            }
        }
    }
    google::ShutdownGoogleLogging();
    return 0;
}