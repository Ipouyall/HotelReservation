#include "decoder.h"

std::string decode::sign_in(std::string& username, std::string& password){
    json j;
    j["username"] = username;
    j["password"] = password;
    return j.dump();
}

std::string decode::check_username_is_free(std::string& username){
    json j;
    j["kind"] = "free_username";
    j["username"] = username;
    return j.dump();
}

std::string decode::sign_up(std::string& username, std::string& password, int balance, std::string& phone, std::string& addr){
    json j;
    j["kind"] = "sign_up";
    j["username"] = username;
    j["password"] = password;
    j["balance"] = balance;
    j["phone"] = phone;
    j["addr"] = addr;
    return j.dump();
}

std::string decode::get_user_info(std::string& token){
    json j;
    j["kind"] = "user_info";
    j["token"] = token;
    return j.dump();
}

std::string decode::get_users(std::string& token){
    json j;
    j["kind"] = "view_users_info";
    j["token"] = token;
    return j.dump();
}

std::string decode::get_rooms_info(std::string& token){
    json j;
    j["kind"] = "view_rooms_info";
    j["token"] = token;
    return j.dump();
}

std::string decode::book_room(std::string& token, int roomID, int n_beds, std::string& check_in, std::string& check_out){
    json j;
    j["kind"] = "booking";
    j["token"] = token;
    j["roomID"] = roomID;
    j["beds_count"] = n_beds;
    j["check_in"] = check_in;
    j["check_out"] = check_out;
    return j.dump();
}

std::string decode::get_reservations(std::string& token){
    json j;
    j["kind"] = "view_reservations";
    j["token"] = token;
    return j.dump();
}

std::string decode::cancel_booking(std::string& token, int roomID, int n_beds){
    json j;
    j["kind"] = "canceling";
    j["token"] = token;
    j["roomID"] = roomID;
    j["beds_count"] = n_beds;
    return j.dump();
}

std::string decode::passing_time(std::string& token, int days){
    json j;
    j["kind"] = "passing_time";
    j["token"] = token;
    j["days"] = days;
    return j.dump();
}

std::string decode::edit_information(std::string& token, std::string& new_pass, std::string phone, std::string addr){
    json j;
    j["kind"] = "edit_information";
    j["token"] = token;
    j["new_pass"] = new_pass;
    j["phone"] = phone;
    j["addr"] = addr;
    return j.dump();
}

std::string decode::leave_room(std::string& token, int roomID){
    json j;
    j["kind"] = "leaving";
    j["token"] = token;
    j["roomID"] = roomID;
    return j.dump();
}

std::string decode::empty_room(std::string& token, int roomID){
    json j;
    j["kind"] = "emptying";
    j["token"] = token;
    j["roomID"] = roomID;
    return j.dump();
}

std::string decode::add_room(std::string& token, int roomID, int max_cap, int price){
    json j;
    j["kind"] = "add_room";
    j["token"] = token;
    j["roomID"] = roomID;
    j["max_cap"] = max_cap;
    j["price"] = price;
    return j.dump();
}

std::string decode::modify_room(std::string& token, int roomID, int max_cap, int price){
    json j;
    j["kind"] = "modify_room";
    j["token"] = token;
    j["roomID"] = roomID;
    j["max_cap"] = max_cap;
    j["price"] = price;
    return j.dump();
}

std::string decode::remove_room(std::string& token, int roomID){
    json j;
    j["kind"] = "remove_room";
    j["token"] = token;
    j["roomID"] = roomID;
    return j.dump();
}

std::string decode::logout(std::string& token){
    json j;
    j["kind"] = "logout";
    j["token"] = token;
    return j.dump();
}
