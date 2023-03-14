#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h> 
#include <stdbool.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string>

#define BUFFER_SIZE 1024*512

int acceptClient(int server_fd);
int setupServer(const char* server_ip, int port, int listen_for=20);
int connectServer(const char* server_ip, int server_port, bool& succeeded);
bool reconnectServer(const char* server_ip, int server_port, int& sockfd);
bool send_message(int sockfd, std::string msg);
bool receive_data(int sockfd, std::string& received);

#endif //SOCKET_UTILS_H