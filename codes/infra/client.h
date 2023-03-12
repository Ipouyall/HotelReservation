#ifndef CODES_DECODER_H
#define CODES_DECODER_H
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace decode {
    std::string sign_in(std::string& username, std::string& password);
    std::string recover(std::string& token);

    std::string check_username_is_free(std::string& username);
    std::string sign_up(std::string& username, std::string& password, int balance, std::string& phone, std::string& addr);

    std::string get_user_info(std::string& token);

    std::string get_users(std::string& token);

    std::string get_rooms_info(std::string& token);

    std::string book_room(std::string& token, int roomID, int n_beds, std::string& check_in, std::string& check_out);

    std::string get_reservations(std::string& token);
    std::string cancel_booking(std::string& token, int roomID, int n_beds);

    std::string passing_time(std::string& token, int days);

    std::string edit_information(std::string& token, std::string& new_pass, std::string phone="", std::string addr="");

    std::string leave_room(std::string& token, int roomID);
    std::string empty_room(std::string& token, int roomID);

    std::string add_room(std::string& token, int roomID, int max_cap, int price);
    std::string modify_room(std::string& token, int roomID, int max_cap, int price);
    std::string remove_room(std::string& token, int roomID);

    std::string logout(std::string& token);
} // decode

namespace encode {
    bool is_username_available(std::string& response);
}

#endif //CODES_DECODER_H
