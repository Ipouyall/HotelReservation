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

int acceptClient(int server_fd);
int setupServer(const char* server_ip, int port);
int connectServer(const char* server_ip, int server_port);

#endif //SOCKET_UTILS_H