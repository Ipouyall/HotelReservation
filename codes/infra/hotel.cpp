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
                    u["reserveDate"],
                    u["checkoutDate"],
            };
            info.users.push_back(user);
        }
        hotel_info.push_back(info);
    }
    return hotel_info;
}

bool HotelRoom::check_user_reserved(int user_id){
    for(int i = 0;i < users.size();i++){
        if(user_id == users[i].id){
            return true;
        }
    }
    return false;
}

HotelManager::HotelManager(){
    rooms = get_hotel_information(DEFAULT_HOTEL_PATH);
}

int HotelManager::search_by_room_num(std::string room_num){
    for(int i = 0;i < rooms.size();i++){
        if(rooms[i].room_number == room_num){
            return i;
        }
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
    if(index != -1){
        return rooms[index].is_full;
    }
    return true;
}

int HotelManager::get_current_capacity(std::string room_num){
    int index = search_by_room_num(room_num);
    if(index != -1){
        return rooms[index].is_full;
    }
}

void HotelManager::change_capacity(std::string room_num, int value){
    int index = search_by_room_num(room_num);
    if(index != -1){
        rooms[index].max_capacity += value;
        rooms[index].current_capacity += value;
    }
}

bool HotelManager::check_user_reserved(std::string room_num, int user_id){
    int index = search_by_room_num(room_num);
    if(index != -1){
        return rooms[index].check_user_reserved(user_id);
    }
    return false;
}
