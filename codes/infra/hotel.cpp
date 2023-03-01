#include "hotel.h"
#include <vector>
#include <glog/logging.h>

vector<HotelRoom> get_hotel_information(string path) {
    LOG(INFO) << "Start initializing hotel information from: [" << path << ']';
    vector<HotelRoom> hotel_info;

    auto jd = readJsonFile(path);
    if (jd == nullptr)
        return hotel_info;

    for (auto j : jd["rooms"]) {
        HotelRoom info;
        info.room_number = j["number"];
        info.is_full = j["status"] == 1;
        info.price_per_user = j["price"];
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