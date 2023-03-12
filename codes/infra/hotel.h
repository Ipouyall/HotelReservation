#ifndef SRC_HOTEL_H
#define SRC_HOTEL_H
#include "filehandler.h"
#include <string>
#include <vector>


#define DEFAULT_HOTEL_PATH "./config/RoomsInfo.json"

typedef struct RoomUser{
    int id;
    int number_of_reservation; // number of beds reserved by user
    std::string reserve_date;
    std::string checkout_date;
} RoomUser;

typedef struct HotelRoom{
    std::string room_number;
    bool is_full; // also called status
    int price_per_bed;
    int max_capacity;
    int current_capacity;
    std::vector<RoomUser> users;
    bool check_user_reserved(int user_id);
} HotelRoom;

std::vector<HotelRoom> get_hotel_information(std::string path);

class HotelManager{
private:
    std::vector<HotelRoom> rooms;
public:
    HotelManager();
    int search_by_room_num(std::string room_num);
    bool room_num_exist(std::string room_num);
    void print_info();
    bool room_is_full(std::string room_num);
    int get_current_capacity(std::string room_num);
    void change_capacity(std::string room_num, int value);
    bool check_user_reserved(std::string room_num, int user_id);
};




#endif //SRC_HOTEL_H
