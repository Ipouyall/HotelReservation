#include "hotel.h"
#include <vector>
#include <glog/logging.h>

std::vector<HotelRoom> get_hotel_information(std::string path) {
    LOG(INFO) << "Start initializing hotel information from: [" << path << ']';
    std::vector<HotelRoom> hotel_info;

    auto jd = readJsonFile(path);
    if (jd == nullptr)
        return hotel_info;

    for (auto j : jd["rooms"]) {
        HotelRoom info;
        info.room_number = j["number"];
        info.is_full = j["status"] == 1;
        info.price_per_bed = j["price"];
        info.max_capacity = j["maxCapacity"];
        info.current_capacity = j["capacity"];
        for (auto u: j["users"]) {
            auto user = RoomUser{
                    u["id"],
                    u["numOfBeds"],
                    dateManager::convert(u["reserveDate"]),
                    dateManager::convert(u["checkoutDate"]),
            };
            info.users.push_back(user);
        }
        hotel_info.push_back(info);
    }
    return hotel_info;
}

bool HotelRoom::check_user_reserved(int user_id){
    return search_user_by_id(user_id) != -1;
}

bool HotelRoom::check_room_available(date::year_month_day check_in, 
                                date::year_month_day check_out, int number_of_bed){
    int availble_capacity;

    for(auto i = date::sys_days(check_in); i < date::sys_days(check_out); i+=date::days{1}){
        availble_capacity = max_capacity;
        for(int j = 0;j < users.size();j++){
            if(date::sys_days(users[j].reserve_date) <= i && i < date::sys_days(users[j].checkout_date))
                availble_capacity -= users[j].number_of_reservation;
            if(number_of_bed > availble_capacity)
                return false;
        }
    }
    return true;
}

int HotelRoom::search_user_by_id(int user_id){
    for(int i = 0;i < users.size();i++){
        if(user_id == users[i].id)
            return i;
    }
    return -1;
}

ReservationDetail HotelRoom::get_user_reservation(int user_id){
    int index = search_user_by_id(user_id);
    if(index == -1)
        return ReservationDetail{};

    return ReservationDetail{
        room_number,
        price_per_bed,
        users[index].number_of_reservation,
        users[index].reserve_date,
        users[index].checkout_date
    };
}

bool HotelRoom::cancel_reservation(int user_id, int num){
    int index = search_user_by_id(user_id);
    if(index == -1)
        return false;
    
    if(num == users[index].number_of_reservation)
        users.erase(users.begin() + index);
    else
        users[index].number_of_reservation -= num;       
    return true;
}

std::vector<RoomUser> HotelRoom::update_date(date::year_month_day new_date){
    std::vector<RoomUser> check_in_users;
    for(int i = 0;i < users.size();i++){
        if(new_date >= users[i].checkout_date){
            users.erase(users.begin() + i);
            i--;
        }
        else if(new_date < users[i].checkout_date && new_date >= users[i].reserve_date){
            check_in_users.push_back(users[i]);
        }
    }
    return check_in_users;
}

void HotelRoom::add_reservation(RoomUser user){
    users.push_back(user);
}

bool HotelRoom::is_user_in(const date::year_month_day& current_date, int user_id){
    int index = search_user_by_id(user_id);
    if(index == -1)
        return false;

    if(current_date >= users[index].reserve_date && current_date < users[index].checkout_date)
        return true;
    return false;
}

void HotelRoom::make_empty(const date::year_month_day& current_date){
    is_full = false;
    current_capacity = max_capacity;
    for(int i = 0;i < users.size();i++){
        if(is_user_in(current_date, users[i].id)){
            users.erase(users.begin() + i);
            i--;
        }
    }
}

bool HotelRoom::left_user(int user_id){
    int index = search_user_by_id(user_id);
    if(index == -1)
        return false;

    is_full = false;
    current_capacity += users[index].number_of_reservation;
    users.erase(users.begin() + index);
    return true;
}

json HotelRoom::get_data_json(bool include_users){
    json j;
    j["number"] = room_number;
    j["status"] = is_full;
    j["total beds"] = max_capacity;
    j["available bed"] = current_capacity;
    j["price(each bed)"] = price_per_bed;
    if(include_users){
        auto users_json = json::array();
        for(int i = 0;i < users.size();i++){
            users_json.push_back({
                {"id", users[i].id},
                {"beds", users[i].number_of_reservation},
                {"check-in", dateManager::get_string(users[i].reserve_date)},
                {"check-out", dateManager::get_string(users[i].checkout_date)}
            });
        }
        j["users"] = users_json;
    }
    return j;
}

HotelManager::HotelManager(){
    rooms = get_hotel_information(DEFAULT_HOTEL_PATH);
}

HotelManager::HotelManager(const date::year_month_day& current_date){
    rooms = get_hotel_information(DEFAULT_HOTEL_PATH);
    update_date(current_date);
}

int HotelManager::search_by_room_num(std::string room_num){
    for(int i = 0;i < rooms.size();i++){
        if(rooms[i].room_number == room_num)
            return i;
    }
    return -1;
}

bool HotelManager::room_num_exist(std::string room_num){
    int index = search_by_room_num(room_num);
    if(index == -1)
        return false;
    return true;
}

void HotelManager::print_info(){
    for(int i = 0;i < rooms.size();i++){
        std::cout << "_________Room = " << rooms[i].room_number<<"_________" << std::endl;
        std::cout << "is full = " << rooms[i].is_full << std::endl;
        std::cout << "price per bed = " << rooms[i].price_per_bed << std::endl;
        std::cout << "max capacity = " << rooms[i].max_capacity << std::endl;
        std::cout << "current capacity = " << rooms[i].current_capacity << std::endl;
        std::cout << "  " << "__Users__" << std::endl;
        for(int j = 0;j < rooms[i].users.size();j++){
            std::cout << "   " << "id = " << rooms[i].users[j].id << std::endl;
            std::cout << "   " << "number of reserve = " << rooms[i].users[j].number_of_reservation << std::endl;
            std::cout << "   " << "check-in date = " << rooms[i].users[j].reserve_date << std::endl;
            std::cout << "   " << "check-out date = " << rooms[i].users[j].checkout_date << std::endl;
        }
    }
}

bool HotelManager::room_is_full(std::string room_num){
    int index = search_by_room_num(room_num);
    if(index == -1)
        return true;

    return rooms[index].is_full;
}

void HotelManager::change_capacity(std::string room_num, int value){
    int index = search_by_room_num(room_num);
    if(index == -1)
        return;
    rooms[index].max_capacity += value;
    rooms[index].current_capacity += value;
}

bool HotelManager::check_user_reserved(std::string room_num, int user_id){
    int index = search_by_room_num(room_num);
    if(index == -1)
        return false;
    return rooms[index].check_user_reserved(user_id);
}

bool HotelManager::add_room(std::string room_num, int capacity, int price_per_bed){
    int index = search_by_room_num(room_num);
    if(room_num == "")
        return false;
    if(index != -1)
        return false;
    HotelRoom new_room;
    new_room.room_number = room_num;
    new_room.max_capacity = capacity;
    new_room.current_capacity = capacity;
    new_room.is_full = false;
    new_room.price_per_bed = price_per_bed;
    rooms.push_back(new_room);
    return true;
}

bool HotelManager::check_room_available(date::year_month_day check_in, date::year_month_day check_out,
                            std::string room_num, int number_of_bed){
    LOG(INFO) << "Checking room available";
    int index = search_by_room_num(room_num);
    if(index == -1){
        LOG(INFO) << "Checking room available finished";
        return false;
    }

    LOG(INFO) << "Checking room available finished";
    return rooms[index].check_room_available(check_in, check_out, number_of_bed);;
}

void HotelManager::update_date(const date::year_month_day& new_date){
    LOG(INFO) << "Updating hotel rooms date to " << new_date;
    for(int i = 0;i < rooms.size();i++){
        rooms[i].is_full = false;
        rooms[i].current_capacity = rooms[i].max_capacity;
        std::vector<RoomUser> check_in_users = rooms[i].update_date(new_date);
        for(int j = 0;j < check_in_users.size();j++){
            rooms[i].current_capacity -= check_in_users[j].number_of_reservation;
        }
        if(rooms[i].current_capacity == 0)
            rooms[i].is_full = true;
    }
    LOG(INFO) << "Updating hotel rooms date to " << new_date << " finished";
}

std::vector<ReservationDetail> HotelManager::get_reservations(int user_id){
    std::vector<ReservationDetail> reservs;
    for(int i = 0;i < rooms.size();i++){
        if(rooms[i].check_user_reserved(user_id))
            reservs.push_back(rooms[i].get_user_reservation(user_id));
    }
    return reservs;
}

bool HotelManager::cancel_reservation(int user_id, std::string room_num, int num){
    int index = search_by_room_num(room_num);
    if(index == -1)
        return false;
    return rooms[index].cancel_reservation(user_id, num);
}

bool HotelManager::cancelation_date_validation(const date::year_month_day& current_date,
                                                 int user_id, std::string room_num){
    int index = search_by_room_num(room_num);
    if(index == -1)
        return false;

    ReservationDetail reserve = rooms[index].get_user_reservation(user_id);
    return current_date < reserve.reserve_date;
}

bool HotelManager::cancelation_capacity_validation(std::string room_num, int user_id, int num){
    int index = search_by_room_num(room_num);
    if(index == -1)
        return false;
    
    ReservationDetail reserve = rooms[index].get_user_reservation(user_id);
    return num <= reserve.number_of_reservation;
}

bool HotelManager::booking_date_validation(const date::year_month_day& current_date, 
                                    date::year_month_day check_in_date, date::year_month_day check_out_date){
    if(check_in_date < current_date || check_in_date >= check_out_date)
        return false;
    return true;
}

int HotelManager::get_total_price(std::string room_num, int number_of_beds){
    int index = search_by_room_num(room_num);
    if(index == -1)
        return 0;

    return number_of_beds * rooms[index].price_per_bed;
}

void HotelManager::book(const date::year_month_day& current_date,
                 std::string room_num, int user_id, int num_of_beds,
                 date::year_month_day check_in_date, date::year_month_day check_out_date){
    
    int index = search_by_room_num(room_num);
    if(index == -1)
        return;

    rooms[index].add_reservation(RoomUser{
        user_id,
        num_of_beds,
        check_in_date,
        check_out_date
    });
    if(check_in_date == current_date)
        update_date(current_date);
}

bool HotelManager::is_user_in_room(const date::year_month_day& current_date,
                                     std::string room_num, int user_id){
    int index = search_by_room_num(room_num);
    if(index == -1) 
        return false;

    return rooms[index].is_user_in(current_date, user_id);
}

bool HotelManager::left_user_room(std::string room_num, int user_id){
    int index = search_by_room_num(room_num);
    if(index == -1)
        return false;

    return rooms[index].left_user(user_id);
}

void HotelManager::make_room_empty(const date::year_month_day& current_date, std::string room_num){
    int index = search_by_room_num(room_num);
    if(index == -1)
        return;
    rooms[index].make_empty(current_date);
}

bool HotelManager::modify_room(std::string room_num, int max_capacity, int price_per_bed){
    if(price_per_bed <= 0 || max_capacity <= 0)
        return false;
    int index = search_by_room_num(room_num);
    if(index == -1)
        return false;

    rooms[index].max_capacity = max_capacity;
    rooms[index].price_per_bed = price_per_bed;
    return true;
}

bool HotelManager::room_is_empty(std::string room_num){
    int index = search_by_room_num(room_num);
    if(index == -1)
        return false;

    return rooms[index].current_capacity == rooms[index].max_capacity;
}

bool HotelManager::remove_room(std::string room_num){
    int index = search_by_room_num(room_num);
    if(index == -1)
        return false;

    rooms.erase(rooms.begin() + index);
    return true;
}

bool HotelManager::modify_validation(std::string room_num, int new_max_capacity){
    int index = search_by_room_num(room_num);
    if(index == -1)
        return false;

    if(rooms[index].current_capacity < rooms[index].max_capacity && 
        new_max_capacity < rooms[index].max_capacity)
        return false;
    return true;
}

std::string HotelManager::get_rooms_data(bool include_users){
    auto rooms_json = json::array();
    for(int i = 0;i < rooms.size();i++){
        rooms_json.push_back(rooms[i].get_data_json(include_users));
    }
    return rooms_json.dump();
}

void HotelManager::save(std::string path){
    nlohmann::ordered_json content;
    auto rooms_json = nlohmann::ordered_json::array();
    for(int i = 0;i < rooms.size();i++){
        nlohmann::ordered_json room_json;
        room_json["number"] = rooms[i].room_number;
        room_json["status"] = (int)rooms[i].is_full;
        room_json["price"] = rooms[i].price_per_bed;
        room_json["maxCapacity"] = rooms[i].max_capacity;
        room_json["capacity"] = rooms[i].current_capacity;

        auto users_json = nlohmann::ordered_json::array();
        for(int j = 0;j < rooms[i].users.size();j++){
            users_json.push_back({
                {"id", rooms[i].users[j].id},
                {"numOfBeds", rooms[i].users[j].number_of_reservation},
                {"reserveDate", dateManager::get_string(rooms[i].users[j].reserve_date)},
                {"checkoutDate", dateManager::get_string(rooms[i].users[j].checkout_date)}
            });
        }
        room_json["users"] = users_json;

        rooms_json.push_back(room_json);
    }
    content["rooms"] = rooms_json;

    writeJsonFile(path, content);
}

