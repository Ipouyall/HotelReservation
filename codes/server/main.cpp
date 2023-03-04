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
#include "infra/socketUtils.h"


#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8081
#define STDOUT 1

const std::string ERROR_CLIENT = "error";


struct Client{
    std::string name;
    int client_fd;
    bool active;
};
Client* newClient() {
    Client* new_client = new Client();
    if (new_client == nullptr)
        exit(EXIT_FAILURE);
    new_client->name = "";
    new_client->active = false;
    new_client->client_fd = -1;
    return new_client;
}

Client* findClient(const std::vector<Client*>& clients, std::string clientName) {
    for (const auto& client : clients) {
        if (client->name == clientName) {
            return client;
        }
    }
    return nullptr;
}
Client* findClientByFd(const std::vector<Client*>& clients, int fd) {
    for (const auto& client : clients) {
        if (client->client_fd == fd) {
            return client;
        }
    }
    return nullptr;
}
void addNewClient(std::string name, std::vector<Client*>& clients, int sock_fd, bool active) {
    auto client = findClient(clients, name);
    if (client == nullptr) {
        clients.push_back(newClient());
        client = clients.back();
    }
    client->name = name;
    client->client_fd = sock_fd;
    client->active = active;
}


void Info(const std::vector<Client*>& clients) {
    for(const auto& client : clients){
        std::cout << "name = " << client->name << std::endl;

        std::cout << "active = " << client->active << std::endl;

        std::cout << "fd = " << client->client_fd << std::endl;
    }
}
void removeRepeatedUser(std::vector<Client*>& clients){
    for(int i = 0;i < clients.size();i++){
        for(int j = 0;j < clients.size();j++){
            if(clients[i]->client_fd == clients[j]->client_fd && clients[j]->name == ""
               && clients[i]->active && clients[j]->active){
                clients.erase(clients.begin() + j);
                j--;
                continue;
            }

        }
    }
}
void removeClient(std::vector<Client*>& clients, std::string name){
    for(int i = 0;i < clients.size();i++){
        if(clients[i]->name == name){
            clients.erase(clients.begin() + i);
            i--;
        }
    }
}

int main(int argc, char *argv[]){
    std::vector<Client*> clients;

    int server_fd, new_socket, max_sd;
    char buffer[1024] = {0};
    fd_set master_set, working_set;


    server_fd = setupServer(SERVER_IP, SERVER_PORT);

    FD_ZERO(&master_set);
    FD_SET(server_fd, &master_set);
    // FD_SET(STDIN, &master_set);

    write(1, "Server is running\n", 19);
    while (true) {
        working_set = master_set;
        memset(buffer, 0, 1024);
        select(FD_SETSIZE, &working_set, NULL, NULL, NULL);
        for(int i = 0;i < FD_SETSIZE;i++){
            if (FD_ISSET(i, &working_set)) {
                if(i == server_fd){
                    std::cout << "new client come = " << i << std::endl;
                    new_socket = acceptClient(i);
                    if(new_socket!=-1)
                        FD_SET(new_socket, &master_set);
                    std::string empty = "";
                    addNewClient(empty, clients, new_socket, true);
                }
                else{
                    int bytes_received = recv(i , buffer, 1024, 0);
                    if (bytes_received == 0) {
                        Client* client = findClientByFd(clients, i);
                        client->active = false;
                        close(i);
                        FD_CLR(i, &master_set);
                        continue;
                    }

                    std::string response = "ok";

                    if(send(i, response.c_str(), strlen(response.c_str()), 0) != -1)
                        write(STDOUT,"Your response sent.\n",21);
                    else
                        write(STDOUT,"Eror on sending your Response!\n",32);



                    char* name = strtok(buffer," ");
                    std::string nameString = name;


                    Client* client = findClientByFd(clients, i);
                    Client* oldClient = findClient(clients, nameString);

                    if(client->name == "" && client->active && oldClient == NULL){
                        client->name = nameString;
                        std::cout << "welcome " << client->name << std::endl;
                    }
                    else if(oldClient != NULL){
                        if(!oldClient->active){
                            std::cout<<"user = " << name << " come again!" << std::endl;
                            oldClient->active = true;
                            oldClient->client_fd = i;
                            removeRepeatedUser(clients);
                        }
                        else{
                            std::cout << "Eror: client = " << name << " alredy is login!" << std::endl;
                            client->name = ERROR_CLIENT;
                            removeClient(clients, ERROR_CLIENT);
                            std::string response = "Sorry you cant login";
                            if(send(i, response.c_str(), strlen(response.c_str()), 0)!=-1)
                                write(STDOUT,"Your response sent.\n",21);
                            else
                                write(STDOUT,"Eror on sending your Response!\n",32);
                            close(i);
                            FD_CLR(i, &master_set);
                        }

                    }
                    else {
                        std::cout << "client name = " << client->name << " said = " << buffer << std::endl;
                    }
                }
            }
        }
    }
    return 0;
}
