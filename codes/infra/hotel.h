#ifndef SRC_HOTEL_H
#define SRC_HOTEL_H
#include "filehandler.h"
#include <string>
#include <vector>

using namespace std;

#define DEFAULT_HOTEL_PATH "./config/RoomsInfo.json"

typedef struct RoomUser{
    int id;
    int number_of_reservation; // number of beds reserved by user
    string reserve_date;
    string checkout_date;
} RoomUser;

typedef struct HotelRoom{
    string room_number;
    bool is_full; // also called status
    int price_per_user;
    int max_capacity;
    int current_capacity;
    vector<RoomUser> users;
} HotelRoom;

vector<HotelRoom> get_hotel_information(string path);

#endif //SRC_HOTEL_H
