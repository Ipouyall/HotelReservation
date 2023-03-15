#include "cli.h"
#include "../infra/client.h"
#include "../infra/socketUtils.h"
#include <glog/logging.h>
#include <sstream>
#include <iostream>
#include <iomanip>

void show_simple_json(json j){
    std::cout << (j["kind"] == "error" ? "\033[1;31m" : "" ) <<
    j["status_code"] << ":" << j["status"] << "\n::::::" <<
    j["message"] << "\n::::::" << j["time"] <<
    "\033[0m" << std::endl;
}

Command::Command() {
    username="";
    token="";
    last_response="";
    is_server_up=true;
    logged_in=false;
}

bool Command::is_server_still_up() {
    return is_server_up;
}

char** Command::initial_state_command_completion(const char* text, int start, int end) {
    static std::vector<std::string> commands = {
            "signin ", "signup ", "exit", "quit", "help", "verbose+", "verbose++", "verbose-", "clear"
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

    if (command == "help") {
        std::cout << help_prompt << std::endl;
        add_history(command.c_str());
    }
    else if (command == "quit" || command == "exit")
        exit(0);
    else if (command == "verbose+")
        FLAGS_minloglevel = 1;
    else if (command == "verbose++")
        FLAGS_minloglevel = 0;
    else if (command == "verbose-")
        FLAGS_minloglevel = 3;
    else if (command == "signup")
        signup(cmd, server_fd);
    else if (command == "signin")
        login(cmd, server_fd);
    else if(command == "clear")
        std::cout << "\x1B[2J\x1B[H";
    else{
        std::cerr << "Unknown command: '" << command << "'" << std::endl;
        std::cout << help_prompt << std::endl;
    }
}

void Command::signup(std::string cmd, int server_fd) {
    std::istringstream stream(cmd);
    std::string command;
    stream >> command;
    LOG(INFO) << "Signup menu...";
    add_history(command.c_str());
    std::string password, phone, address;
    int balance;
    if (stream.eof()) {
        std::cerr << "Wrong command format!" << std::endl;
        return;
    }
    stream >> username;
    if (!stream.eof() || username=="" || username=="signup")
    {
        std::cerr << "Wrong command format!" << std::endl;
        return;
    }
    std::string request = decode::check_username_is_free(username);
    bool sent = send_message(server_fd, request);
    if (!sent)
        return;
    is_server_up = receive_data(server_fd,last_response);
    if (!is_server_up)
        return;
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

void Command::login(std::string cmd, int server_fd) {
    std::istringstream stream(cmd);
    std::string command;
    stream >> command;
    LOG(INFO) << "Signin menu...";
    add_history(command.c_str());
    stream >> username;
    if (username == "signin")
    {
        std::cerr << "Wrong command format!" << std::endl;
        return;
    }
    stream >> password;
    std::string request = decode::sign_in(username, password);
    LOG(INFO) << "Sending login data...";
    bool sent = send_message(server_fd, request);
    if (!sent)
        return;
    is_server_up = receive_data(server_fd,last_response);
    if (!is_server_up)
        return;
    json j = json::parse(last_response);
    show_simple_json(j);
    if (j["kind"]=="error"){
        return;
    }
    token = j["token"];
    logged_in=true;
    clear_history();
}

void Command::recover_state(int server_fd) {
    if (!logged_in)
        return;
    LOG(INFO) << "Recovering client's state...";
    std::string request = decode::sign_in(username, password);
    bool sent = send_message(server_fd, request);
    if (!sent)
        return;
    std::string srv_response;
    is_server_up = receive_data(server_fd,srv_response);
    json j = json::parse(srv_response);
    if (j["kind"]=="error"){
        LOG(ERROR) << "Couldn't recover previous state!";
        logged_in=false;
        token="";
        return;
    }
    token = j["token"];
    logged_in=true;
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
        prompt += "+ clear/exit/quit/help";
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
        exit(0);
    else if (command == "verbose+")
        FLAGS_minloglevel = 1;
    else if (command == "verbose++")
        FLAGS_minloglevel = 0;
    else if (command == "verbose-")
        FLAGS_minloglevel = 3;
    else if (command == "clear")
        std::cout << "\x1B[2J\x1B[H";
    else if (command=="0" || command=="0_logout" || command=="logout")
        logout(cmd, server_fd);
    else if (command=="1" || command=="1_view_user_information" || command=="view_user_information")
        view_user_info(cmd, server_fd);
    else if (command=="2" || command=="2_view_all_users" || command=="view_all_users")
        view_users(cmd, server_fd);
    else if (command=="3" || command=="3_view_rooms_information" || command=="view_rooms_information")
        view_rooms(cmd, server_fd);
    else if (command=="4" || command=="4_booking" || command=="booking")
        book_room(cmd, server_fd);
    else if (command=="5" || command=="5_cancelling" || command=="cancelling")
        cancel_reservation(cmd, server_fd);
    else if (command=="6" || command=="6_pass_day" || command=="pass_day")
        pass_day(cmd, server_fd);
//    else if (command=="7" || command=="7_edit_information" || command=="edit_information")
//        edit_information(cmd, server_fd);
//    else if (command=="8" || command=="8_leaving_room" || command=="leaving_room")
//        leave_room(cmd, server_fd);
//    else if (command=="9" || command=="9_rooms" || command=="rooms")
//        hotel_management(cmd, server_fd);
    else
        std::cerr << "Unknown command: '" << command << "'\n" <<
                     "::use <help> command to learn about commands" << std::endl;
    std::cout << "Press <Enter> to continue...";
    std::cin.get();
}

void Command::logout(std::string cmd, int server_fd) {
    std::istringstream stream(cmd);
    std::string command;
    stream >> command;
    LOG(INFO) << "Logging out...";
    add_history(command.c_str());
    std::string request = decode::logout(token);
    bool sent = send_message(server_fd, request);
    if (!sent)
        return;
    is_server_up = receive_data(server_fd,last_response);
    if (!is_server_up)
        return;
    json j = json::parse(last_response);
    show_simple_json(j);
    token="";
    logged_in=false;
    username="";
}

void Command::view_user_info(std::string cmd, int server_fd) {
    std::istringstream stream(cmd);
    std::string command;
    stream >> command;
    LOG(INFO) << "Getting user data...";
    add_history(command.c_str());
    std::string request = decode::get_user_info(token);
    bool sent = send_message(server_fd, request);
    if (!sent)
        return;
    is_server_up = receive_data(server_fd,last_response);
    if (!is_server_up)
        return;
    json j = json::parse(last_response);
    show_simple_json(j);
    std::string udata = j["data"];
    json ud = json::parse(udata);
    if(j["kind"]=="success") {
        std::cout<< "User account's information:" << std::endl;
        print_user_info(ud);
        std::cout<< "---" << std::endl;
    }
}

void Command::view_users(std::string cmd, int server_fd) {
    std::istringstream stream(cmd);
    std::string command;
    stream >> command;
    LOG(INFO) << "Getting all users data...";
    add_history(command.c_str());
    std::string request = decode::get_users(token);
    bool sent = send_message(server_fd, request);
    if (!sent)
        return;
    is_server_up = receive_data(server_fd,last_response);
    if (!is_server_up)
        return;
    json j = json::parse(last_response);
    show_simple_json(j);
    std::string udata = j["data"];
    if(j["kind"]=="success")
        print_users_info(udata);
}

void Command::view_rooms(std::string cmd, int server_fd) {
    std::istringstream stream(cmd);
    std::string command;
    stream >> command;
    LOG(INFO) << "Getting rooms information...";
    add_history(command.c_str());
    std::string request = decode::get_rooms_info(token);
    bool sent = send_message(server_fd, request);
    if (!sent)
        return;
    is_server_up = receive_data(server_fd,last_response);
    if (!is_server_up)
        return;
    json j = json::parse(last_response);
    show_simple_json(j);
    std::string udata = j["data"];
    if(j["kind"]=="success")
        print_rooms_info(udata);
}

void Command::book_room(std::string cmd, int server_fd) {
    std::istringstream stream(cmd);
    std::string command;
    stream >> command;
    LOG(INFO) << "Booking a room...";
    std::cout <<
              "Command format: book <room_number> <beds_count> <check_in_date(dd-mm-yyyy)> <check_out_date(dd-mm-yyyy)>" <<
              std::endl;
    int beds_count;
    std::string roomID, check_in, check_out;
    char* line = readline("> ");
    if (line == nullptr) {
        std::cout << "Error in reading prompt!" << std::endl;
        return;
    }
    std::string line_str(line), t_cmd;
    free(line);
    std::istringstream line_stream(line_str);
    line_stream >> t_cmd;
    if(t_cmd != "book") {
        std::cout << "Wrong format, command should start with 'book'!" << std::endl;
        return;
    }
    if(line_stream.eof()) {
        std::cout << "You need to specify arguments!" << std::endl;
        return;
    }
    line_stream >> roomID;
    if(line_stream.eof()) {
        std::cout << (
                roomID != "" ?
                "Just provided roomNumber, you should specify all arguments!" :
                "Just provided ???, you should specify all arguments!") << std::endl;
        return;
    }
    line_stream >> beds_count;
    if(line_stream.eof()){
        std::cout << "Just provided roomNum+bedCount, you should specify all arguments!" << std::endl;
        return;
    }
    line_stream >> check_in;
    if(line_stream.eof()) {
        std::cout << (
                check_in != "" ?
                "Just provided roomNum+bedCount+checkInDate, you should specify all arguments!" :
                "Just provided roomNum+bedCount+???, you should specify all arguments!") << std::endl;
        return;
    }
    line_stream >> check_out;
    if (check_out == "") {
        std::cout << "Just provided roomNum+bedCount+checkInDate+???, you should specify all arguments!" << std::endl;
        return;
    }
    std::string temp;
    line_stream >> temp;
    if(!line_stream.eof() && temp!="") {
        std::cout << "Too many argument!" << std::endl;
        return;
    }
    add_history(line_str.c_str());
    std::string request = decode::book_room(token, roomID, beds_count, check_in, check_out);
    bool sent = send_message(server_fd, request);
    if (!sent)
        return;
    is_server_up = receive_data(server_fd,last_response);
    if (!is_server_up)
        return;
    json j = json::parse(last_response);
    show_simple_json(j);
}

template<typename T>
void print_element(T t, const int& width)
{
    std::cout << std::left << std::setw(width) << std::setfill(' ') << t;
}

void print_user_info(json user_data){
    std::string valid_rows[] = {
            "id", "username", "role", "status", "purse","phone number", "address"
    };
    for (auto& key : valid_rows) {
        if (!user_data.contains(key))
            continue;
        print_element(key, 15);
        print_element(":", 2);
        print_element(user_data[key], 30);
        std::cout << std::endl;
    }
}

void print_users_info(std::string users_data){
    std::cout<< "Users information:" << std::endl;
    auto ud = json::parse(users_data);
    for (std::string jj : ud) {
        json j = json::parse(jj);
        std::cout<< "+++" << std::endl;
        print_user_info(j);
    }
    std::cout<< "---" << std::endl;
}

void print_room_info(json room_data){
    std::string valid_rows[] = {
            "number", "status", "total beds", "available bed", "price(each bed)",
    };
    for (auto& key : valid_rows) {
        if (!room_data.contains(key))
            continue;
        print_element(key, 15);
        print_element(":", 2);
        print_element(room_data[key], 30);
        std::cout << std::endl;
    }
    if (!room_data.contains("users"))
        return;
    std::string valid_user_rows[] = {
            "id", "beds", "check-in", "check-out"
    };
    std::cout << "Users:" << std::endl;
    for (std::string jj : room_data["users"]) {
        json j = json::parse(jj);
        std::cout<< "*-  ";
        for (auto& key : valid_user_rows) {
            if (!j.contains(key))
                continue;
            if (key != "id")
                print_element("", 4);
            print_element(key, 15);
            print_element(":", 2);
            print_element((j[key]), 30);
            std::cout << std::endl;
        }
    }
}

void Command::cancel_reservation(std::string cmd, int server_fd) {
    std::istringstream stream(cmd);
    std::string command;
    stream >> command;
    LOG(INFO) << "Canceling a reservation/ requesting to view reservations...";
    add_history(command.c_str());
    std::string request = decode::get_reservations(token);
    bool sent = send_message(server_fd, request);
    if (!sent)
        return;
    is_server_up = receive_data(server_fd,last_response);
    if (!is_server_up)
        return;
    json j = json::parse(last_response);
    show_simple_json(j);
    if(j["kind"] != "success")
        return;
    std::string data = j["data"];
    print_reservations(data);
    std::cout << "command format: cancel <room number> <number of bed(s)>" << std::endl;
    std::string roomID;
    int beds_count;
    char* line = readline("> ");
    if (line == nullptr)
        return;
    std::string line_str(line), t_cmd;
    free(line);
    std::istringstream line_stream(line_str);
    line_stream >> t_cmd;
    if(line_stream.eof() || t_cmd != "cancel") return;
    line_stream >> roomID;
    if(line_stream.eof()) return;
    line_stream >> beds_count;
    if(!line_stream.eof()) return;
    add_history(line_str.c_str());

    request = decode::cancel_booking(token, roomID, beds_count);
    sent = send_message(server_fd, request);
    if (!sent)
        return;
    is_server_up = receive_data(server_fd,last_response);
    if (!is_server_up)
        return;
    j = json::parse(last_response);
    show_simple_json(j);
}

void print_rooms_info(std::string rooms_data){
    std::cout<< "- Rooms information:" << std::endl;
    auto rd = json::parse(rooms_data);
    for (std::string jj : rd) {
        json j = json::parse(jj);
        std::cout<< "*********************************" << std::endl;
        print_room_info(j);
    }
    std::cout<< "------------------------------" << std::endl;
}

void print_reservations(std::string reservations_data){
    std::cout<< "Reservations:" << std::endl;
    std::string keys[] = {
            "room number", "price(per bed)", "bed(s) you have", "check in", "check out"
    };
    auto rd = json::parse(reservations_data);
    for (std::string jj : rd) {
        json j = json::parse(jj);
        std::cout<< "*****" << std::endl;
        for (auto& key : keys) {
            if (!j.contains(key))
                continue;
            if (key != "room number")
                print_element("", 4);
            print_element(key, 15);
            print_element(":", 2);
            print_element((j[key]), 30);
            std::cout << std::endl;
        }
    }
    std::cout<< "-----" << std::endl;
}

void Command::pass_day(std::string cmd, int server_fd) {
    std::istringstream stream(cmd);
    std::string command;
    stream >> command;
    LOG(INFO) << "Updating system's date...";
    add_history(command.c_str());
    std::cout << "command format: passDay <number of days>" << std::endl;
    int days;
    char* line = readline("> ");
    if (line == nullptr)
        return;
    std::string line_str(line), t_cmd;
    std::istringstream line_stream(line_str);
    line_stream >> t_cmd;
    if(line_stream.eof() || t_cmd != "passDay") {
        std::cout << "Invalid command: Command should starts with 'passDay'!" << std::endl;
        return;
    }
    line_stream >> days;
    if(!line_stream.eof()) {
        std::cout << "Invalid command: Too many arguments!" << std::endl;
        return;
    }
    add_history(line_str.c_str());
    std::string request = decode::passing_time(token, days);
    bool sent = send_message(server_fd, request);
    if (!sent)
        return;
    is_server_up = receive_data(server_fd,last_response);
    if (!is_server_up)
        return;
    json j = json::parse(last_response);
    show_simple_json(j);
}
