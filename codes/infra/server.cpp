#include "server.h"
#include "socketUtils.h"
#include <iostream>
#include <glog/logging.h>

serverConfig get_server_config(std::string path) {
    LOG(INFO) << "Start initializing server configuration from: [" << path << ']';
    auto conf = serverConfig{};

    auto j = readJsonFile(path);
    if (j == nullptr)
        return conf;

    conf.host_name = j["hostName"];
    conf.port = j["commandChannelPort"];

    return conf;
}

Server::Server() {
    LOG(INFO) << "Initializing Server...";
    set_time();
    config = get_server_config(DEFAULT_SERVER_PATH);
    um = UserManager();
    hm = HotelManager(today_date);
    errors = readJsonFile(DEFAULT_ERRORS_PATH);
    fd = setupServer(config.host_name.c_str(), config.port);
}

int Server::get_fd() { return fd;}

std::string Server::diagnose(std::string command, int client_fd) {
    LOG(INFO) << "Diagnosing incoming request:" << command;
    if(command == "client_dead") {
        um.client_dead(client_fd);
        return "";
    }
    json json_data_in = json::parse(command);
    if (json_data_in.contains("kind") == false) {
        return "";
    }
    std::string cmd = json_data_in["kind"];
    std:: string rsp="";
    if(cmd == "sign_in")
        rsp = sign_in(json_data_in, um, client_fd);
    else if(cmd == "free_username")
        rsp = is_uname_available(json_data_in, um);
    else if(cmd == "sign_up")
        rsp = signup(json_data_in, um);
    else if(cmd == "logout")
        rsp = logout(json_data_in, um);
    else if(cmd == "user_info")
        rsp = view_user_information(json_data_in, um);
    else if(cmd == "view_users_info")
        rsp = view_all_users(json_data_in, um);
    else if(cmd == "view_rooms_info")
        rsp = view_rooms_info(json_data_in, um, hm);
    else if(cmd == "booking")
        rsp = book_a_room(json_data_in, um, hm);
    else if (cmd == "view_reservations")
        rsp = view_reservations(json_data_in, um, hm);
    else if (cmd == "canceling")
        rsp = cancel_a_room(json_data_in, um, hm);
    else if (cmd == "passing_time")
        rsp = pass_days(json_data_in, um, hm);
    else if (cmd == "edit_information")
        rsp = edit_user_info(json_data_in, um);
    else if (cmd == "leaving")
        rsp = leave_room(json_data_in, um, hm);
    else if (cmd == "emptying")
        rsp = empty_room(json_data_in, um, hm);
    else if (cmd == "add_room")
        rsp = add_a_room(json_data_in, um, hm);
    else if (cmd == "modify_room")
        rsp = modify_a_room(json_data_in, um, hm);
    else if (cmd == "remove_room")
        rsp = remove_a_room(json_data_in, um, hm);
    return rsp;
}

json Server::response(std::string kind, std::string status_code, std::string msg){
    std::string status_msg = errors[status_code];
    json rsp;
    rsp["kind"] = kind;
    rsp["status_code"] = status_code;
    rsp["status"] = status_msg;
    rsp["message"] = msg;
    rsp["time"] = dateManager::get_string(today_date);
    return rsp;
}

std::string Server::sign_in(json& j_in, UserManager& um, int fd){
    LOG(INFO) << "Login request received on (" << fd << ")";
    std::string username = j_in["username"];
    std::string password = j_in["password"];
    json rsp;
    std::string token = "";
    if (!um.user_validation(username, password))
    {
        rsp = response(
                "error", "430", "Sign_in failed due to invalid password/username or you logged in before."
        );
        LOG(WARNING) << "Login request from (" << fd << ") failed!";
    }
    else
    {
        token = um.login(username, fd);
        rsp =  response(
                "success", "230", "You logged in successfully."
        );
        rsp["token"] = token;
        rsp["privilege"] = um.get_role(token) == UserRole::ADMIN;
        LOG(INFO) << "Login request from (" << fd << ") succeeded.";
    }
    save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), fd,
                         rsp["status_code"], rsp["status"], rsp["message"], "signin");
    return rsp.dump();
}

std::string Server::is_uname_available(json &j_in, UserManager &um) {
    LOG(INFO) << "Checking username is available";
    std::string username = j_in["username"];
    bool available = !um.username_exist(username);
    json rsp;

    if (available)
    {
        rsp = response("success", "311", "Just validated you username, please complete your registration.");
        LOG(INFO) << "Username is available";
    }
    else
    {
        rsp = response("error", "451", "Username exists, try another one.");
        LOG(WARNING) << "Username is available";
    }
    return rsp.dump();
}

std::string Server::signup(json& j, UserManager& um) {
    // TODO: client, after validating username is free, should again send uname,
    LOG(INFO) << "Signup request received";
    std::string username = j["username"];
    std::string pass = j["password"];
    int ba = j["balance"];
    std::string pn = j["phone"];
    std::string addr = j["addr"];
    json rsp;

    auto succeeded = um.signup(username, pass, ba, pn, addr);
    if(succeeded)
    {
        LOG(INFO) << "User signed up successfully";
        rsp = response("success", "231", "We honor to announce you are part of our community from now on.");
    }
    else
    {
        LOG(WARNING) << "User signup failed";
        rsp = response("error", "503", "Signing up failed, make sure you entered valid data.");
    }
    save_server_log(today_date, "server/", rsp["kind"]=="success", "", fd,
                         rsp["status_code"], rsp["status"], rsp["message"], "signup");
    return rsp.dump();
}

std::string Server::logout(json &j_in, UserManager &um) {
    // TODO: client fd should be cleared from fd-set
    LOG(INFO) << "Logout request received";
    std::string token = j_in["token"];
    std::string username = um.get_username(token);
    auto succeeded = um.logout(token);
    json rsp;

    rsp = response("success", "201", "Hope to see you later!");

    save_server_log(today_date, "server/", rsp["kind"]=="success", username, -1,
                         rsp["status_code"], rsp["status"], rsp["message"], "logout");    
    return rsp.dump();
}

std::string Server::view_user_information(json &j_in, UserManager &um) {
    LOG(INFO) << "View-user-info request received";
    std::string token = j_in["token"];
    std::string data = um.get_user_data(token);
    json rsp;
    if (data=="")
        rsp = response("error", "000", "Couldn't find any data from this user, you may need to login again or edit your info.");
    else
        rsp = response("success", "001", "Here is your information");
    rsp["data"] = data;
    save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), -1,
                         rsp["status_code"], rsp["status"], rsp["message"], "View user information"); 
    return rsp.dump();
}

std::string Server::view_all_users(json &j_in, UserManager &um) {
    LOG(INFO) << "New request for view all users data received";
    std::string token = j_in["token"];
    std::string data = um.get_users_data(token);
    json rsp;
    if(data=="")
        rsp = response("error", "403", "You don't have required access for this functionality");
    else
        rsp = response("success", "001", "Here is all users in our hotel");
    rsp["data"] = data;
    save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), -1,
                         rsp["status_code"], rsp["status"], rsp["message"], "View all users"); 
    return rsp.dump();
}

std::string Server::view_rooms_info(json &j_in, UserManager &um, HotelManager &hm) {
    LOG(INFO) << "New request for view all rooms data received";
    std::string token = j_in["token"];
    UserRole role = um.get_role(token);
    json rsp;
    if (role == UserRole::NONE){
        rsp = response("error", "403", "You don't have required access for this functionality");
        save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), -1,
                         rsp["status_code"], rsp["status"], rsp["message"], "View rooms information"); 
        return rsp.dump();
    }
    bool admin_access = role == UserRole::ADMIN;
    std::string data = hm.get_rooms_data(admin_access);
    if (data=="")
        rsp = response("error", "000", "Operation failed, please try again later!");
    else
        rsp = response("success", "001", "Here is our rooms information");
    rsp["data"] = data;
    save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), -1,
                         rsp["status_code"], rsp["status"], rsp["message"], "View rooms information"); 
    return rsp.dump();
}

std::string Server::book_a_room(json &j_in, UserManager &um, HotelManager &hm) {
    LOG(INFO) << "New request for booking a room received";
    std::string token = j_in["token"];
    UserRole role = um.get_role(token);
    json rsp;
    if (role != UserRole::USER){
        rsp = response("error", "403", "Only users can book a room");
        save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), -1,
                         rsp["status_code"], rsp["status"], rsp["message"], "Booking"); 
        return rsp.dump();
    }
    std::string room_id = j_in["roomID"], check_in = j_in["check_in"], check_out = j_in["check_out"];
    int bed_count = j_in["beds_count"], user_id = um.get_id(token);
    if (!dateManager::valid_format(check_in) || !dateManager::valid_format(check_out)){
        rsp = response("error", "503", "User <dd-mm-yyyy> formatted date");
        save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), -1,
                         rsp["status_code"], rsp["status"], rsp["message"], "Booking"); 
        return rsp.dump();
    }
        

    auto check_in_date = dateManager::convert(check_in);
    auto check_out_date = dateManager::convert(check_out);

    if (!hm.room_num_exist(room_id)){
        rsp = response("error", "101", "Room number doesn't exist");
        save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), -1,
                         rsp["status_code"], rsp["status"], rsp["message"], "Booking"); 
        return rsp.dump();
    }
    if(hm.check_user_reserved(room_id, user_id)){
        rsp = response("error", "503", "You should cancel your previous reservation");
        save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), -1,
                         rsp["status_code"], rsp["status"], rsp["message"], "Booking"); 
        return rsp.dump();
    }
    if (!um.have_enough_money(token, hm.get_total_price(room_id, bed_count, check_in_date, check_out_date))){
        rsp =response("error", "108", "You don't have enough money to book this room");
        save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), -1,
                         rsp["status_code"], rsp["status"], rsp["message"], "Booking"); 
        return rsp.dump();
    }
    if (!hm.booking_date_validation(today_date, check_in_date, check_out_date)){
        rsp = response("error", "503", "Error in entered dates");
        save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), -1,
                         rsp["status_code"], rsp["status"], rsp["message"], "Booking"); 
        return rsp.dump();
    }
    if (!hm.check_room_available(check_in_date, check_out_date, room_id, bed_count)){
        rsp = response("error", "109", "Room doesn't meet your request");
        save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), -1,
                         rsp["status_code"], rsp["status"], rsp["message"], "Booking"); 
        return rsp.dump();
    }
    auto cost = hm.get_total_price(room_id, bed_count, check_in_date, check_out_date);
    hm.book(today_date, room_id, user_id, bed_count, check_in_date, check_out_date);
    um.reduce_balance(token,cost);
    json rsp = response("success", "110", "Room reserved successfully");
    save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), -1,
                         rsp["status_code"], rsp["status"], rsp["message"], "Booking"); 
    return rsp.dump();
}

std::string Server::view_reservations(json &j_in, UserManager &um, HotelManager &hm) {
    LOG(INFO) << "New request for view reservations received";
    std::string token = j_in["token"];
    UserRole role = um.get_role(token);
    json rsp;
    if (role != UserRole::USER){
        rsp = response("error", "403", "Only users can cancel a reservation");
        save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), -1,
                         rsp["status_code"], rsp["status"], rsp["message"], "Get reservations"); 
        return rsp.dump();
    }
    int user_id = um.get_id(token);
    std::string data = hm.view_reservations(user_id);
    if (data=="")
        rsp = response("error", "000", "Operation failed, please try again later!");
    else
        rsp = response("success", "001", "Here is your reservations");
    rsp["data"] = data;
    save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), -1,
                         rsp["status_code"], rsp["status"], rsp["message"], "Get reservations"); 
    return rsp.dump();
}

std::string Server::cancel_a_room(json &j_in, UserManager &um, HotelManager &hm) {
    LOG(INFO) << "New request for canceling a room received";
    std::string token = j_in["token"];
    UserRole role = um.get_role(token);
    if (role != UserRole::USER){
        json rsp = response("error", "403", "Only users can cancel a reservation");
        save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), -1,
                         rsp["status_code"], rsp["status"], rsp["message"], "Cancelling"); 
        return rsp.dump();
    }
    std::string room_id = j_in["roomID"];
    int beds_count = j_in["beds_count"], user_id = um.get_id(token);

    json rsp;
    if (!hm.room_num_exist(room_id)){
        rsp = response("error", "101", "Room number doesn't exist");
        save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), -1,
                         rsp["status_code"], rsp["status"], rsp["message"], "Cancelling"); 
        return rsp.dump();
    }
    if (!hm.check_user_reserved(room_id, user_id) ||
        !hm.cancelation_capacity_validation(room_id, user_id, beds_count)){
        rsp = response("error", "102",
                        "Something is wrong with room's number or number of beds you want to cancel");
        save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), -1,
                         rsp["status_code"], rsp["status"], rsp["message"], "Cancelling"); 
        return rsp.dump();
    }
    if (!hm.cancelation_date_validation(today_date, user_id, room_id)){
        rsp = response("error", "401", "You can't cancel your reservation now");
        save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), -1,
                         rsp["status_code"], rsp["status"], rsp["message"], "Cancelling"); 
        return rsp.dump();
    }
    int price_back = hm.cancel_reservation(user_id, room_id, beds_count);
    um.increase_balance(token, price_back);
    rsp = response("success", "110", "Reservation canceled successfully");
    save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), -1,
                         rsp["status_code"], rsp["status"], rsp["message"], "Cancelling"); 
    return rsp.dump();

}

std::string Server::pass_days(json &j_in, UserManager &um, HotelManager &hm) {
    LOG(INFO) << "New request for passing days received";
    std::string token = j_in["token"];
    UserRole role = um.get_role(token);
    json rsp;
    if (role != UserRole::ADMIN){
        rsp = response("error", "403", "Only admins can pass days");
        save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), -1,
                         rsp["status_code"], rsp["status"], rsp["message"], "Pass day"); 
        return rsp.dump();
    }
    int days = j_in["days"];
    today_date = dateManager::inc_days(today_date, days);
    hm.update_date(today_date);
    rsp = response("success", "110", "System's date updated successfully");
    save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), -1,
                         rsp["status_code"], rsp["status"], rsp["message"], "Pass day"); 
    return rsp.dump();
}

std::string Server::edit_user_info(json &j_in, UserManager &um) {
    LOG(INFO) << "New request for editing user info received";
    std::string token = j_in["token"];
    std::string new_pass, new_phone, new_addr;
    new_pass = j_in.contains("pass") ? j_in["pass"] : "";
    new_phone = j_in.contains("phone") ? j_in["phone"] : "";
    new_addr = j_in.contains("addr") ? j_in["addr"] : "";
    json rsp;
    if (um.edit_information(token, new_pass, new_phone, new_addr)){
        rsp = response("success", "110", "User information updated successfully");
        save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), -1,
                         rsp["status_code"], rsp["status"], rsp["message"], 
                         new_pass != "" ? "Edit information|password" :
                         new_phone != "" ? "Edit information|phone" :
                         new_addr != "" ? "Edit information|address" : ""); 
        return rsp.dump();
    }
    else{
        rsp = response("error", "503", "Operation failed, please try again later");
        save_server_log(today_date, "server/", rsp["kind"]=="success", um.get_username(token), -1,
                         rsp["status_code"], rsp["status"], rsp["message"], 
                         new_pass != "" ? "Edit information|password" :
                         new_phone != "" ? "Edit information|phone" :
                         new_addr != "" ? "Edit information|address" : ""); 
        return rsp.dump();
    }
}

std::string Server::leave_room(json &j_in, UserManager &um, HotelManager &hm) {
    LOG(INFO) << "New request for leaving a room received";
    std::string token = j_in["token"];
    if(um.get_role(token) != UserRole::USER)
        return response("error", "403", "Only users can leave a room!").dump();
    int user_id = um.get_id(token);
    std::string roomID = j_in["roomID"];
    if(!hm.room_num_exist(roomID))
        return response("error", "503", "That wasn't a valid room").dump();
    if(!hm.is_user_in_room(today_date, roomID, user_id))
        return response("error", "102", "You are not in this room (at least, yet)").dump();
    hm.left_user_room(roomID, user_id);
    return response("success", "413", "Hope you enjoyed our hotel, bye").dump();
}

std::string Server::empty_room(json &j_in, UserManager &um, HotelManager &hm) {
    LOG(INFO) << "New request for emptying a room received";
    std::string token = j_in["token"];
    if(um.get_role(token) != UserRole::ADMIN)
        return response("error", "403", "Only admins can manage a room!").dump();
    std::string roomID = j_in["roomID"];
    if(!hm.room_num_exist(roomID))
        return response("error", "101", "That isn't a valid room").dump();
    hm.make_room_empty(today_date, roomID);
    return response("success", "413", "All users has kicked out").dump();
}

std::string Server::add_a_room(json &j_in, UserManager &um, HotelManager &hm) {
    LOG(INFO) << "New request for adding a room received";
    std::string token = j_in["token"];
    if(um.get_role(token) != UserRole::ADMIN)
        return response("error", "403", "Only admins can manage a room!").dump();
    std::string roomID = j_in["roomID"];
    int max_cap = j_in["max_cap"];
    int price = j_in["price"];
    if(hm.room_num_exist(roomID))
        return response("error", "111", "This room already exists").dump();
    if(!hm.add_room(roomID, max_cap, price))
        return response("error", "111", "Couldn't add room").dump();
    return response("success", "104", "Room (" + roomID + ") added successfully").dump();
}

std::string Server::modify_a_room(json &j_in, UserManager &um, HotelManager &hm) {
    LOG(INFO) << "New request for modifying a room received";
    std::string token = j_in["token"];
    if(um.get_role(token) != UserRole::ADMIN)
        return response("error", "403", "Only admins can manage a room!").dump();
    std::string roomID = j_in["roomID"];
    int max_cap = j_in["max_cap"];
    int price = j_in["price"];
    if(!hm.room_num_exist(roomID))
        return response("error", "101", "This isn't a valid room!").dump();
    if(!hm.modify_validation(roomID, max_cap))
        return response("error", "109", "Room should be empty for modification!").dump();
    if(!hm.modify_room(roomID, max_cap, price))
        return response("error", "000", "Couldn't modify room!!!").dump();
    return response("success", "105", "Room (" + roomID + ") modified successfully").dump();
}

std::string Server::remove_a_room(json &j_in, UserManager &um, HotelManager &hm) {
    LOG(INFO) << "New request for removing a room received";
    std::string token = j_in["token"];
    if (um.get_role(token) != UserRole::ADMIN)
        return response("error", "403", "Only admins can manage a room!").dump();
    std::string roomID = j_in["roomID"];
    if (!hm.room_num_exist(roomID))
        return response("error", "101", "This isn't a valid room").dump();
    if (!hm.room_is_empty(roomID))
        return response("error", "109", "Room should be empty for removing!").dump();
    hm.remove_room(roomID);
    return response("success", "106", "Room (" + roomID + ") removed successfully").dump();
}

void Server::rewrite_data() {
    LOG(INFO) << "Rewriting data...";
    um.save(DEFAULT_USERS_PATH);
    hm.save(DEFAULT_HOTEL_PATH);
}

void Server::set_time() {
    LOG(INFO) << "Setting time...";
    std::string t_time;
    std::cout << "Enter today's date (dd-mm-yyyy): ";
    std::cin >> t_time;
    while (!dateManager::valid_format(t_time)) {
        std::cout << "Invalid date format, try again!" << std::endl;
        std::cout << "Enter today's date (dd-mm-yyyy): ";
        std::cin >> t_time;
    }
    today_date = dateManager::convert(t_time);
}
