# HotelReservation/ Hotel Manager & Hotel Room

### RoomUser

First we started with describing struct for users who reserved room named `RoomUser` as follows:

```c++
typedef struct RoomUser{
    int id;
    int number_of_reservation;
    date::year_month_day reserve_date;
    date::year_month_day checkout_date;
} RoomUser;
```

### ReservationDetail

The next struct that is useful for getting information about details of user's reservation is `ReservationDetail` :

```c++
typedef struct ReservationDetail{
    std::string room_number;
    int price_per_bed;
    int number_of_reservation;
    date::year_month_day reserve_date;
    date::year_month_day checkout_date;
} ReservationDetail;
```

### HotelRoom

Also we define `HotelRoom` for managing and containin `RoomUser`s :

```c++
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
    bool capacity_validation(int new_capacity);
} HotelRoom;
```

and now we describe it's methods:

```c++
bool check_user_reserved(int user_id);
```

- Check that user with id = `user_id` reserved in this room or not.

```c++
int search_user_by_id(int user_id);
```
- Search based on `user_id` given and return index of user or -1 if doesn't exist.

```c++
ReservationDetail get_user_reservation(int user_id);
```
- Return details about reservation of user based on his id and if user doesn't have reservation empty struct will be return.

```c++
bool check_room_available(date::year_month_day check_in, date::year_month_day check_out, int number_of_bed);
```

- Check that user can reserve this room for duration between `check_in` and `check_out` (`return true`) or not (`return false`).


```c++
std::vector<RoomUser> update_date(date::year_month_day new_date);
```

- Update reservers base on `new_date` given. It Removes reservations that their check-out date reached and return the users that are int room on `new_date`.

```c++
bool cancel_reservation(int user_id, int num);
```

- Cancel `num` of beds reservation for user with id = `user_id` then return `true` if successfully done.

```c++
void add_reservation(RoomUser user);
```

- Add reservation.

```c++
bool is_user_in(const date::year_month_day& current_date, int user_id);
```

- Check that user with id = `user_id` will be in room at date = `current_date` (`return true`) or not (`return false`).

```c++
bool left_user(int user_id);
```

- Remove user reseravtion with id = `user_id` and update it's capacity.

```c++
void make_empty(const date::year_month_day& current_date);
```

- Make the room empty and delete users on `current_date`

```c++
json get_data_json(bool include_users);
```
- Return room information in json format and if `include_users` set to `true`, it adds users reservation's information in json.

```c++
bool capacity_validation(int new_capacity);
```
- Check new capacity with other reservation then `return true` if is ok and less all of them.

### HotelManager
This class manage everything about reservation such as booking a room, canceling reservation, leaving room , etc.

- First of all it initialize `std::vector<HotelRoom> rooms` by reading `RoomsInfo.json`.
- It can also get current date in it's constructor for updating reservations read from `RoomsInfo.json`.

```c++
class HotelManager{
private:
    std::vector<HotelRoom> rooms;
public:
    HotelManager();
    HotelManager(const date::year_month_day& current_date);
    int search_by_room_num(std::string room_num);
    bool room_num_exist(std::string room_num);
    bool room_is_full(std::string room_num);
    void change_capacity(std::string room_num, int value);
    bool check_user_reserved(std::string room_num, int user_id);
    bool add_room(std::string room_num, int capacity, int price_per_bed);
    bool check_room_available(date::year_month_day check_in, date::year_month_day check_out,
                            std::string room_num, int number_of_bed);
    void update_date(const date::year_month_day& new_date);
    std::vector<ReservationDetail> get_reservations(int user_id);
    int cancel_reservation(int user_id, std::string room_num, int num);
    bool cancelation_date_validation(const date::year_month_day& current_date, int user_id, std::string room_num);
    bool cancelation_capacity_validation(std::string room_num, int user_id, int num);
    bool booking_date_validation(const date::year_month_day& current_date, 
                                    date::year_month_day check_in_date, date::year_month_day check_out_date);
    int get_total_price(std::string room_num, int number_of_beds, 
                                    date::year_month_day check_in_date, date::year_month_day check_out_date);
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
```

```c++
int search_by_room_num(std::string room_num);
```

- Search based on `room_num` given and return index of room or -1 if doesn't exist.

```c++
bool room_num_exist(std::string room_num);
```

- Check that if `room_num` given exist in rooms(`return true`) or not (`return false`).

```c++
bool room_is_full(std::string room_num);
```

- Check that if room with num = `room_num` given is full (`return true`) or not (`return false`).


```c++
void change_capacity(std::string room_num, int value);
```

- Change `current_capacity` and `max_capacity` of the room.

```c++
bool check_user_reserved(std::string room_num, int user_id);
```

- Check that if user with id = `user_id` given has reservation for room with num = `room_num` (`return true`) or not (`return false`).

```c++
bool add_room(std::string room_num, int capacity, int price_per_bed);
```

- Add room with num = `room_num`, cpacity = `capacity` and price = `price_per_bed` then return `true` if successfully done.

```c++
bool check_room_available(date::year_month_day check_in, date::year_month_day check_out,
                            std::string room_num, int number_of_bed);
```

- Check that user can reserve `number_of_bed` beds for room with num = `room_num` for duration between `check_in` and `check_out` (`return true`) or not (`return false`).

```c++
void update_date(const date::year_month_day& new_date);
```
- Call update date for all rooms and update their `capacity` and `is_full`.

```c++
std::vector<ReservationDetail> get_reservations(int user_id);
```
- Collect user's all reservation with id = `user_id` and then return it.

```c++
int cancel_reservation(int user_id, std::string room_num, int num);
```
- Call `cancel_reservation` with for user with id =  `user_id` and `num` of beds, for room with num = `room_num`

```c++
bool cancelation_date_validation(const date::year_month_day& current_date, int user_id, std::string room_num);
```
- Check that the check in date for the reservation that users with id = `user_id` submitted for room with num = `room_num` (`return false`) or not (`return true`).

```c++
bool cancelation_capacity_validation(std::string room_num, int user_id, int num);
```
- Check thst user with id = `user_id` has less or equal beds resavation than `num`(`return false`) or not (`return true`).

```c++
bool booking_date_validation(const date::year_month_day& current_date, 
                                date::year_month_day check_in_date, date::year_month_day check_out_date);
```
- Check that check-in date and check-out date are valid or not.

```c++
int get_total_price(std::string room_num, int number_of_beds, 
                                date::year_month_day check_in_date, date::year_month_day check_out_date);
```
- Calculate price for room with num = `room_num` with `number_of_beds` bed for duration between `check_in_date` and `check_out_date`.

```c++
void book(const date::year_month_day& current_date,
                std::string room_num, int user_id, int num_of_beds,
                date::year_month_day check_in_date, date::year_month_day check_out_date);
```
- Add reaervation for user with id = `user_id` for room with num = `room_num`.

```c++
bool is_user_in_room(const date::year_month_day& current_date, std::string room_num, int user_id);
```
- Check that user with id = `user_id` is inside room with num = `room_num` at `current_date`.

```c++
bool left_user_room(std::string room_num, int user_id);
```
- Call left_user method with `user_id` for room with num = `room_num`.

```c++
void make_room_empty(const date::year_month_day& current_date, std::string room_num);
```
- Remove users who are in the room at current date.

```c++
bool modify_validation(std::string room_num, int new_max_capacity);
```
- Check that can change room's max capacity with `new_max_capacity` or not. If `new_max_capacity` is less than old max capacity and there is some users in room it return `false`.

```c++
bool modify_room(std::string room_num, int max_capacity, int price_per_bed);
```
- Change room max_capacity and price for beds then return `true` if successfully done.

```c++
bool room_is_empty(std::string room_num);
```
- Check that in room with num = `room_num` is empty(`return true`) or not.

```c++
bool remove_room(std::string room_num);
```
- Remove room with num = `room_num`.

```c++
std::string get_rooms_data(bool include_users);
```
- Return all rooms data inculding cpacity, max capacity, price per bed and num and if `include_users`, users reservation will be added to that then it return these informations in json format string.

example
```json
[
  {
    "available bed": 0,
    "number": "301",
    "price(each bed)": 100,
    "status": false,
    "total beds": 1,
    "users": [
      {
        "beds": 1,
        "check-in": "27-02-2023",
        "check-out": "28-02-2023",
        "id": 2
      },
      {
        "beds": 1,
        "check-in": "25-02-2023",
        "check-out": "26-02-2023",
        "id": 3
      }
    ]
  }
]
```
```c++
void save(std::string path);
```
- Save state of hotel manger in `RoomsInfo.json`.
```c++
bool capacity_validation(std::string room_num, int new_capacity)
```
- Check new capacity for room with num = `room_num` with other reservation then `return true` if is ok and less all of them.
