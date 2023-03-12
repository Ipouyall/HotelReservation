//
// Created by Pooya Sadeghi on 2023-03-12.
//

#include "cli.h"
#include "../infra/client.h"
#include "../infra/socketUtils.h"
#include <glog/logging.h>
#include <sstream>

void show_simple_json(json j){
    std::cout << (j["kind"] == "error" ? "\033[1;31m" : "" )
    << j["status_code"] << ":" << j["status"] << "\n:::" << j["message"] <<
    "\033[0m" << std::endl;
}

char** Command::initial_state_command_completion(const char* text, int start, int end) {
    static std::vector<std::string> commands = {"signin", "signup", "exit", "quit", "help", "verbose+", "verbose++", "verbose-"};
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
void Command::initial_state_execute_command(const std::string& cmd, int server_fd) {
    std::string help_prompt = "You haven't authorized yet, try following:\n";
    help_prompt += "- signin <username> <password>\n";
    help_prompt += "- signup <username>\n";
    help_prompt += "         <password>\n";
    help_prompt += "         <purse>\n";
    help_prompt += "         <phone>\n";
    help_prompt += "         <address>\n";
    help_prompt += "- exit\n";
    help_prompt += "- quit\n";
    help_prompt += "- help";
    std::istringstream stream(cmd);
    std::string command;
    stream >> command;

    if (command == "help")
    {
        std::cout << help_prompt << std::endl;
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
        std::string username, password, phone, address;
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
        // TODO: get response and act due to that
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
        // TODO: get response and act due to that
    }
    else
    {
        std::cerr << "Unknown command: " << command << std::endl;
        std::cout << help_prompt << std::endl;
    }
}

//int main() {
//    // Set the command completion function
//    rl_attempted_completion_function = (CPPFunction*)command_completion;
//
//    // Read commands from the user
//    char* input = nullptr;
//    while ((input = readline("> ")) != nullptr) {
//        // Add the input to the history
//        add_history(input);
//
//        // Execute the command
//        std::string command(input);
//        execute_command(command);
//
//        // Free the input buffer
//        free(input);
//    }
//
//    return 0;
//}