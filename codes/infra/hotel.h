#ifndef SRC_HOTEL_H
#define SRC_HOTEL_H
#include "filehandler.h"
#include "dateManager.h"
#include <string>
#include <vector>
#include <date/date.h>

#define DEFAULT_HOTEL_PATH "./config/RoomsInfo.json"

typedef struct RoomUser{
    int id;
    int number_of_reservation; // number of beds reserved by user
    date::year_month_day reserve_date;
    date::year_month_day checkout_date;
} RoomUser;

typedef struct ReservationDetail{
    std::string room_number;
    int price_per_bed;
    int number_of_reservation; // number of beds reserved by user
    date::year_month_day reserve_date;
    date::year_month_day checkout_date;
} ReservationDetail;

typedef struct HotelRoom{
    std::string room_number;
    bool is_full; // also called status
    int price_per_bed;
    int max_capacity;
    int current_capacity;
    std::vector<RoomUser> users;
    bool check_user_reserved(int user_id);
    int search_user_by_id(int user_id);
    ReservationDetail get_user_reservation(int user_id);
    bool check_room_available(date::year_month_day check_in, 
                                date::year_month_day check_out, int number_of_bed);
    std::vector<RoomUser> update_date(date::year_month_day new_date);
    bool cancel_reservation(int user_id, int num);
    void add_reservation(RoomUser user);
    bool is_user_in(const date::year_month_day& current_date, int user_id);
    bool left_user(int user_id);
    void make_empty(const date::year_month_day& current_date);
    json get_data_json(bool include_users);
} HotelRoom;




std::vector<HotelRoom> get_hotel_information(std::string path);



class HotelManager{
private:
    std::vector<HotelRoom> rooms;
public:
    HotelManager();
    HotelManager(const date::year_month_day& current_date);
    int search_by_room_num(std::string room_num);
    bool room_num_exist(std::string room_num);
    void print_info();
    bool room_is_full(std::string room_num);
    void change_capacity(std::string room_num, int value);
    bool check_user_reserved(std::string room_num, int user_id);
    bool add_room(std::string room_num, int capacity, int price_per_bed);
    bool check_room_available(date::year_month_day check_in, date::year_month_day check_out,
                            std::string room_num, int number_of_bed);
    void update_date(const date::year_month_day& new_date);
    std::vector<ReservationDetail> get_reservations(int user_id);
    bool cancel_reservation(int user_id, std::string room_num, int num);
    bool cancelation_date_validation(const date::year_month_day& current_date, int user_id, std::string room_num);
    bool cancelation_capacity_validation(std::string room_num, int user_id, int num);
    bool booking_date_validation(const date::year_month_day& current_date, 
                                    date::year_month_day check_in_date, date::year_month_day check_out_date);
    int get_total_price(std::string room_num, int number_of_beds);
    void book(const date::year_month_day& current_date,
                 std::string room_num, int user_id, int num_of_beds,
                 date::year_month_day check_in_date, date::year_month_day check_out_date);
    bool is_user_in_room(const date::year_month_day& current_date, std::string room_num, int user_id);
    bool left_user_room(std::string room_num, int user_id);
    void make_room_empty(const date::year_month_day& current_date, std::string room_num);
    bool modify_validation(std::string room_num, int new_max_capacity);
    bool modify_room(std::string room_num, int max_capacity, int price_per_bed);
    bool room_is_empty(std::string room_num);
    bool remove_room(std::string room_num);
    std::string get_rooms_data(bool include_users);
    void save(std::string path);
};




#endif //SRC_HOTEL_H
