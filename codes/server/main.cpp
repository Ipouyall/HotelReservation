#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <vector>
#include <glog/logging.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "../infra/socketUtils.h"
#include "../infra/server.h"

void signalHandler(int signum) {
    LOG(WARNING) << "Program terminated unsafely, user 'exit' command to save server state!!!";
    google::ShutdownGoogleLogging();
    exit(1);
}

// TODO: sever should save tokens and restore them when coming up
int main(int argc, char *argv[]) {
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();
    FLAGS_colorlogtostderr = true;
    FLAGS_log_prefix = true;
    FLAGS_logtostderr = true;
    FLAGS_alsologtostderr = true;

    int server_fd, new_socket, max_sd;
    std::string bufferString;
    fd_set master_set, working_set;
    auto server = Server();

    signal(SIGINT, signalHandler);

    server_fd = server.get_fd();

    FD_ZERO(&master_set);
    FD_SET(server_fd, &master_set);
    FD_SET(STDIN_FILENO, &master_set);
    LOG(INFO) << "Server is running...";

    while (true) {
        working_set = master_set;
        select(FD_SETSIZE, &working_set, NULL, NULL, NULL);
        for (int i = 0; i < FD_SETSIZE; i++) {
            if (!FD_ISSET(i, &working_set))
                continue;
            if (i == STDIN_FILENO){
                std::string command;
                std::cin >> command;
                if (command == "exit"){
                    server.shout_down(0);
                    google::ShutdownGoogleLogging();
                    exit(0);
                }
            }
            if (i == server_fd) {
                new_socket = acceptClient(i);
                if (new_socket != -1)
                    FD_SET(new_socket, &master_set);
            } else {
                bufferString = "";
                bool is_up = receive_data(i, bufferString);
                if (!is_up) { // Client has left
                    FD_CLR(i, &master_set);
                    LOG(INFO) << "Client (" << i << ") gone";
                    server.diagnose("client_dead", i);
                    close(i);
                    continue;
                }

                std::string response = server.diagnose(bufferString, i);
                if (send(i, response.c_str(), strlen(response.c_str()), 0) != -1)
                    LOG(INFO) << "Your response sent";
                else
                    LOG(WARNING) << "Error on sending your Response!";
            }
        }
    }
    google::ShutdownGoogleLogging();
    return 0;
}
