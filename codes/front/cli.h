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
    std::string token;
    std::string last_response;

public:
    Command();
    void initial_menu(const std::string& cmd, int server_fd);
    std::string get_token();
    std::string get_last_response();

private:
    char** initial_state_command_completion(const char* text, int start, int end);
    void initial_state_execute_command(const std::string& cmd, int server_fd);
};

void show_simple_json(json j);

#endif //CODES_CLI_H
