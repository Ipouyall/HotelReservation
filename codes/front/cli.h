#ifndef CODES_CLI_H
#define CODES_CLI_H
#include <iostream>
#include <string>
#include <vector>
#include <readline/readline.h>
#include <nlohmann/json.hpp>
#include "../infra/dateManager.h"

using json = nlohmann::json;

class Command{
private:
    std::string username;
    std::string password;
    std::string token;
    std::string last_response;
    bool logged_in;
    bool is_server_up;
    bool privilege_access;

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

    void help();
    void logout(std::string command, int server_fd);
    void login(std::string command, int server_fd);
    void signup(std::string command, int server_fd);
    void view_user_info(std::string command, int server_fd);
    void view_users(std::string command, int server_fd);
    void view_rooms(std::string command, int server_fd);
    void book_room(std::string command, int server_fd);
    void cancel_reservation(std::string command, int server_fd);
    void pass_day(std::string command, int server_fd); // TODO: Implement this
    void edit_information(std::string command, int server_fd); // TODO: Implement this
    void leave_room(std::string command, int server_fd); // TODO: Implement this
    void hotel_management(std::string command, int server_fd); // TODO: Implement this

};

void show_simple_json(json j);
void print_user_info(json user_data);
void print_users_info(std::string users_data);
void print_room_info(json room_data, bool filter_available, bool filter_empty);
void print_rooms_info(std::string rooms_data, bool filter_available, bool filter_empty);
void print_reservations(std::string reservations_data);

#endif //CODES_CLI_H
