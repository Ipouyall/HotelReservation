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


int main(int argc, char const *argv[]) {
    google::InitGoogleLogging(argv[0]);
    FLAGS_colorlogtostderr = true;
    FLAGS_log_prefix = true;
    FLAGS_logtostderr = true;
    FLAGS_alsologtostderr = true;
    LOG(INFO) << "Initializing Client...";

    auto server_info = get_server_config(DEFAULT_SERVER_PATH);
    int sockfd;
    char buffer[BUFFER_SIZE];

    bool connected;
    sockfd = connectServer(server_info.host_name.c_str(), server_info.port, connected);

    while (!connected){
        LOG(WARNING) << "Reconnecting in 2 seconds";
        sleep(2);
        connected = reconnectServer(server_info.host_name.c_str(), server_info.port, sockfd);
    }

    fd_set master_set, working_set;
    FD_ZERO(&master_set);
    FD_SET(sockfd, &master_set);
    FD_SET(STDIN_FILENO, &master_set);

    while (true) {
        working_set = master_set;
        select(FD_SETSIZE, &working_set, NULL, NULL, NULL);

        for (int i = 0; i < FD_SETSIZE; i++) {
            if (FD_ISSET(i, &working_set)) {
                if (i == STDIN_FILENO) {
                    memset(buffer, 0, BUFFER_SIZE);
                    read(STDIN_FILENO, buffer, BUFFER_SIZE);
                    buffer[strlen(buffer)-1] = '\0';

                    if(send(sockfd, buffer, strlen(buffer), 0) != -1)
                        LOG(INFO) << "Your response sent to server.";
                    else
                        LOG(WARNING) << "Error on sending your response to server!";
                }
                else if (i == sockfd) {
                    memset(buffer, 0, BUFFER_SIZE);
                    int bytes_received = recv(sockfd , buffer, BUFFER_SIZE, 0);
                    if (bytes_received == 0) {
                        close(sockfd);
                        FD_CLR(sockfd, &master_set);
                        LOG(INFO) << "Server closed!";
                        return 0;
                    }
                    LOG(INFO) << "Server said: " << buffer;
                }
                else {
                    LOG(INFO) << "Unknown file descriptor: " << i ;
                }
            }
        }
    }
    google::ShutdownGoogleLogging();
    return 0;
}