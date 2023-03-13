//
// Created by Pooya Sadeghi on 2023-03-12.
//

#ifndef CODES_CLI_H
#define CODES_CLI_H
#include <iostream>
#include <string>
#include <vector>
#include <readline/readline.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Command{
private:
    std::string username;
    std::string password;
    std::string token;
    std::string last_response;
    bool logged_in;
    bool is_server_up;

public:
    Command();
    void activate_autocompletion();
    void execute_command(std::string command, int server_fd);
    std::string get_token();
    std::string get_last_response();
    void recover_state(int server_fd);
    bool is_server_still_up();

private:
    static char** initial_state_command_completion(const char* text, int start, int end);
    static char** reservation_command_completion(const char* text, int start, int end);
    void execute_reservation_command(const std::string& cmd, int server_fd);
    void execute_initial_state_command(const std::string& cmd, int server_fd);
};

void show_simple_json(json j);
void print_user_info(json user_data);

#endif //CODES_CLI_H
