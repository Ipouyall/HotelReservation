#include "cli.h"
#include "../infra/client.h"
#include "../infra/socketUtils.h"
#include <glog/logging.h>
#include <sstream>
#include <iostream>

void show_simple_json(json j){
    std::cout << (j["kind"] == "error" ? "\033[1;31m" : "" ) <<
    j["status_code"] << ":" << j["status"] << "\n:::" <<
    j["message"] << "\033[0m" << std::endl;
}

Command::Command() {
    username="";
    token="";
    last_response="";
    is_server_up=true;
    logged_in=false;
}

char** Command::initial_state_command_completion(const char* text, int start, int end) {
    static std::vector<std::string> commands = {
            "signin", "signup", "exit", "quit", "help", "verbose+", "verbose++", "verbose-", "clear"
    };
    const char* prefix = rl_line_buffer;
    std::vector<std::string> matches;

    // Find all commands that match the current prefix
    for (const auto& command : commands) {
        if (strncmp(command.c_str(), prefix, end) == 0) {
            matches.push_back(command);
        }
    }
    // Return the matches
    if (matches.size() > 0) {
        // Allocate an array of char* to hold the matches
        char** matches_array = (char**)malloc(sizeof(char*) * (matches.size() + 1));
        for (size_t i = 0; i < matches.size(); i++) {
            matches_array[i] = strdup(matches[i].c_str());
        }
        matches_array[matches.size()] = nullptr;

        return matches_array;
    } else {
        return nullptr;
    }
}

// Command execution function
void Command::execute_initial_state_command(const std::string& cmd, int server_fd) {
    std::string help_prompt = "You haven't authorized yet, try following:\n";
    help_prompt += "- signin <username> <password>\n";
    help_prompt += "- signup <username>\n";
    help_prompt += "         <password>\n";
    help_prompt += "         <purse>\n";
    help_prompt += "         <phone>\n";
    help_prompt += "         <address>\n";
    help_prompt += "- exit\n";
    help_prompt += "- quit\n";
    help_prompt += "- help\n";
    help_prompt += "- clear";
    std::istringstream stream(cmd);
    std::string command;
    stream >> command;

    if (command == "help")
    {
        std::cout << help_prompt << std::endl;
        add_history(command.c_str());
    }
    else if (command == "quit" || command == "exit")
    {
        exit(0);
    }
    else if (command == "verbose+")
    {
        FLAGS_minloglevel = 1;
    }
    else if (command == "verbose++")
    {
        FLAGS_minloglevel = 0;
    }
    else if (command == "verbose-")
    {
        FLAGS_minloglevel = 3;
    }
    else if (command == "signup")
    {
        LOG(INFO) << "Signup menu...";
        add_history(command.c_str());
        std::string password, phone, address;
        int balance;
        stream >> username;
        if (username == "signup")
        {
            std::cerr << "Wrong command format!" << std::endl;
            return;
        }
        std::string request = decode::check_username_is_free(username);
        bool sent = send_message(server_fd, request);
        if (!sent)
            return;
        is_server_up = receive_data(server_fd,last_response);
        json j = json::parse(last_response);
        if (j["kind"]=="error"){
            show_simple_json(j);
            return;
        }
        std::cout << "password: ";
        std::cin >> password;
        std::cout << "purse: ";
        std::cin >> balance;
        std::cout << "phone: ";
        std::cin >> phone;
        address = readline("address: ");
        request = decode::sign_up(username, password,balance, phone, address);
        sent = send_message(server_fd, request);
        if (!sent)
            return;
        is_server_up = receive_data(server_fd,last_response);
        j = json::parse(last_response);
        show_simple_json(j);
    }
    else if (command == "signin")
    {
        LOG(INFO) << "Signin menu...";
        add_history(command.c_str());
        std::string pass;
        stream >> username;
        if (username == "signin")
        {
            std::cerr << "Wrong command format!" << std::endl;
            return;
        }
        stream >> pass;
        std::string request = decode::sign_in(username, pass);
        LOG(INFO) << "Sending login data...";
        bool sent = send_message(server_fd, request);
        if (!sent)
            return;
        is_server_up = receive_data(server_fd,last_response);
        json j = json::parse(last_response);
        show_simple_json(j);
        if (j["kind"]=="error"){
            return;
        }
        token = j["token"];
        logged_in=true;
    }
    else if(command == "clear")
    {
        std::cout << "\x1B[2J\x1B[H";
    }
    else
    {
        std::cerr << "Unknown command: '" << command << "'" << std::endl;
        std::cout << help_prompt << std::endl;
    }
}

void Command::activate_autocompletion() {
    if (logged_in)
    {
        LOG(INFO) << "Reservation menu is activating...";
        rl_attempted_completion_function = (CPPFunction *) reservation_command_completion;
        std::string prompt = "Welcome " + username + "!\nHow can I help you?\n";
        prompt += "1. View user information\n";
        prompt += "2. View all users\n";
        prompt += "3. View rooms information\n";
        prompt += "4. Booking\n";
        prompt += "5. Cancelling\n";
        prompt += "6. Pass day\n";
        prompt += "7. Edit information\n";
        prompt += "8. Leaving room\n";
        prompt += "9. Rooms\n";
        prompt += "0. Logout\n";
        prompt += "+ clear\n";
        prompt += "+ exit\n";
        prompt += "+ quit\n";
        prompt += "+ help";
        std::cout << prompt << std::endl;
    }
    else {
        LOG(INFO) << "Initial menu is activating...";
        rl_attempted_completion_function = (CPPFunction *) initial_state_command_completion;
    }
}

void Command::execute_command(std::string command, int server_fd) {
    if(logged_in){
        execute_reservation_command(command, server_fd);
    }
    else
    {
        execute_initial_state_command(command, server_fd);
    }
}

char** Command::reservation_command_completion(const char* text, int start, int end) {
    static std::vector<std::string> commands = {
            "", "view_",
            "1_view_user_information", "view_user_information",
            "2_view_all_users", "view_all_users",
            "3_view_rooms_information", "view_rooms_information",
            "4_booking", "booking",
            "5_cancelling", "cancelling",
            "6_pass_day", "pass_day",
            "7_edit_information", "edit_information",
            "8_leaving_room", "leaving_room",
            "9_rooms", "rooms",
            "0_logout", "logout",
            "signup",
            "exit", "quit", "help", "verbose+", "verbose++", "verbose-", "clear"
    };
    const char* prefix = rl_line_buffer;
    std::vector<std::string> matches;

    // Find all commands that match the current prefix
    for (const auto& command : commands) {
        if (strncmp(command.c_str(), prefix, end) == 0) {
            matches.push_back(command);
        }
    }
    // Return the matches
    if (matches.size() > 0) {
        // Allocate an array of char* to hold the matches
        char** matches_array = (char**)malloc(sizeof(char*) * (matches.size() + 1));
        for (size_t i = 0; i < matches.size(); i++) {
            matches_array[i] = strdup(matches[i].c_str());
        }
        matches_array[matches.size()] = nullptr;

        return matches_array;
    } else {
        return nullptr;
    }
}

void Command::execute_reservation_command(const std::string& cmd, int server_fd) {
    std::istringstream stream(cmd);
    std::string command;
    stream >> command;

    if (command == "help")
    {
        std::cout << "Implement help prompt" << std::endl;
    }
    else if (command == "quit" || command == "exit")
    {
        exit(0);
    }
    else if (command == "verbose+")
    {
        FLAGS_minloglevel = 1;
    }
    else if (command == "verbose++")
    {
        FLAGS_minloglevel = 0;
    }
    else if (command == "verbose-")
    {
        FLAGS_minloglevel = 3;
    }
    else if (command == "clear")
    {
        std::cout << "\x1B[2J\x1B[H";
    }
    else if (command=="0" || command=="0_logout" || command=="logout")
    {
        LOG(INFO) << "Logging out...";
        std::string request = decode::logout(token);
        bool sent = send_message(server_fd, request);
        if (!sent)
            return;
        is_server_up = receive_data(server_fd,last_response);
        json j = json::parse(last_response);
        show_simple_json(j);
        token="";
        logged_in=false;
        username="";
    }
    else
    {
        std::cerr << "Unknown command: '" << command << "'\n" <<
                     "use <help> command to learn about commands" << std::endl;
    }
}
