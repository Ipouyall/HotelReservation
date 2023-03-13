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



struct Client{
    int client_fd;

    Client(int fd){ client_fd = fd;}
};

void add_new_client(std::vector<Client>& clients, int sock_fd) {
    clients.push_back(Client(sock_fd));
}

void signalHandler(int signum) {
    LOG(WARNING) << "Received signal " << signum << ", terminating the program...";
    google::ShutdownGoogleLogging();
    exit(signum);
}

// TODO: logout user when client is deactivated, set fd=-1 in UM and handle this in search by fd
int main(int argc, char *argv[]) {
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();
    FLAGS_colorlogtostderr = true;
    FLAGS_log_prefix = true;
    FLAGS_logtostderr = true;
    FLAGS_alsologtostderr = true;
    LOG(INFO) << "Initializing Server...";

    signal(SIGINT, signalHandler);

    std::vector<Client> clients;
    int server_fd, new_socket, max_sd;
    std::string bufferString;
    fd_set master_set, working_set;
    auto server = Server();
    auto users = UserManager();

    server_fd = server.get_fd();

    FD_ZERO(&master_set);
    FD_SET(server_fd, &master_set);
    LOG(INFO) << "Server is running...";

    while (true) {
        working_set = master_set;
        select(FD_SETSIZE, &working_set, NULL, NULL, NULL);
        for (int i = 0; i < FD_SETSIZE; i++) {
            if (!FD_ISSET(i, &working_set))
                continue;
            if (i == server_fd) {
                new_socket = acceptClient(i);
                if (new_socket != -1)
                    FD_SET(new_socket, &master_set);
                add_new_client(clients, new_socket);
            } else {
                bufferString = "";
                bool is_up = receive_data(i, bufferString);
                if (!is_up) { // Client has left
                    close(i);
                    FD_CLR(i, &master_set);
                    LOG(INFO) << "Client gone";
                    continue;
                }

                std::cout << "client fd = " << i << " said = " << bufferString << std::endl;
                std::string response = server.diagnose(bufferString, users, i);
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

// TODO: having a mechanism to automatically logout user, when client is terminated