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

using namespace std;

int connect_to_server(serverConfig& server_info, int max_attempts=10) {
    int sockfd, attempt=1;

    bool connected;
    sockfd = connectServer(server_info.host_name.c_str(), server_info.port, connected);

    while (!connected && attempt < max_attempts){
        attempt++;
        int wait_time = 2 + (attempt/2);
        LOG(WARNING) << "Reconnecting in " << wait_time << " seconds";
        sleep(wait_time);
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

void setUP_client(char const * name) {
    google::InitGoogleLogging(name);
    google::InstallFailureSignalHandler();
    FLAGS_minloglevel=0;
    FLAGS_colorlogtostderr = true;
    FLAGS_log_prefix = true;
    FLAGS_logtostderr = true;
    FLAGS_alsologtostderr = true;
    LOG(INFO) << "Initializing Client...";
    signal(SIGINT, signalHandler);
}

int main(int argc, char const *argv[]) {
    setUP_client(argv[0]);

    auto server_info = get_server_config(DEFAULT_SERVER_PATH);
    int sockfd = connect_to_server(server_info);

    std::string buffer;

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
                    std::cin >> buffer;
                    send_message(sockfd, buffer);
                }
                else if (i == sockfd) { // sth from server is reached
                    buffer = "";
                    bool is_up = receive_data(sockfd, buffer);
                    if (!is_up) { // when server is down
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