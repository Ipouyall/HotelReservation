#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include "infra/socketUtils.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8081
#define BUFFER_SIZE 1024

using namespace std;


int main(int argc, char const *argv[]) {
    int sockfd;
    char buffer[BUFFER_SIZE];

    sockfd = connectServer(SERVER_IP, SERVER_PORT);

    string name;
    cin >> name;

    fd_set master_set, working_set;
    FD_ZERO(&master_set);
    FD_SET(sockfd, &master_set);
    FD_SET(STDIN_FILENO, &master_set);

    send(sockfd, name.c_str(), strlen(name.c_str()), 0);
    cout << "Your name is: " << name << endl;

    while (true) {
        working_set = master_set;
        select(FD_SETSIZE, &working_set, NULL, NULL, NULL);

        for (int i = 0; i < FD_SETSIZE; i++) {
            if (FD_ISSET(i, &working_set)) {
                if (i == STDIN_FILENO) {
                    memset(buffer, 0, BUFFER_SIZE);
                    read(STDIN_FILENO, buffer, BUFFER_SIZE);
                    buffer[strlen(buffer)-1] = '\0';

                    char* command = strtok(buffer, " ");
                    if (strcmp(command, "send") == 0) {
                        char* message = strtok(NULL, "\n");
                        send(sockfd, message, strlen(message), 0);
                    }
                    else if (strcmp(command, "exit") == 0) {
                        close(sockfd);
                        FD_CLR(sockfd, &master_set);
                        cout << "Goodbye!" << endl;
                        return 0;
                    }
                    else {
                        cout << "Invalid command!" << endl;
                    }
                }
                else if (i == sockfd) {
                    memset(buffer, 0, BUFFER_SIZE);
                    int bytes_received = recv(sockfd , buffer, BUFFER_SIZE, 0);
                    if (bytes_received == 0) {
                        close(sockfd);
                        FD_CLR(sockfd, &master_set);
                        cout << "Server closed!" << endl;
                        return 0;
                    }
                    cout << "Server said: " << buffer << endl;
                }
                else {
                    cout << "Unknown file descriptor: " << i << endl;
                }
            }
        }
    }

    return 0;
}