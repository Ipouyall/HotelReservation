#include "infra/server.h"
#include "infra/hotel.h"
#include "infra/user.h"
#include "infra/dateManager.h"
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <glog/logging.h>
#include <date/date.h>
#include <iomanip>

void signalHandler(int signum) {
    LOG(INFO) << "Received signal " << signum << ", terminating the program...";
    google::ShutdownGoogleLogging();
    exit(signum);
}

int main(int argc, char **argv) { // using just for test functionalities
    google::InitGoogleLogging(argv[0]);
    signal(SIGINT, signalHandler);
    FLAGS_colorlogtostderr = true;
    FLAGS_log_prefix = true;
    FLAGS_logtostderr = true;
    FLAGS_alsologtostderr = true;

    LOG(INFO) << "Initializing program ...";

    auto c = get_server_config(DEFAULT_SERVER_PATH);
    std::cout << c.host_name << ':' << c.port << std::endl;


    auto h = get_hotel_information(DEFAULT_HOTEL_PATH);
    for (auto _h : h){
        std::cout << _h.room_number << ' ' << _h.max_capacity - _h.current_capacity << std::endl;
    }

    auto u = read_users_data(DEFAULT_USERS_PATH);
    for (auto _u:u){
        std::cout << _u.id << ' ' << (_u.privilege ? _u.username : _u.phone_number) << std::endl;
    }

    std::cout << "Token1: " << generate_token() << std::endl;
    std::cout << "Token2: " << generate_token() << std::endl;

    UserManager manager;
    manager.save("./config/UsersInfo2.json");

    std::cout << "before insertion" << std::endl;
    manager.printINfo();

    manager.signup("sfdgdfgffd", "pokopkp54", 50, "0912235222", "werw street, ookok valley");
    std::string token = manager.login("sfdgdfgffd", 4);
    manager.reduce_balance(token, 20);

    std::cout << "after insertion" << std::endl;

    manager.printINfo();
    


    HotelManager h_manager;
    std::cout << h_manager.get_rooms_data(true) << std::endl;
    h_manager.save("./config/RoomsInfo2.json");
    std::string date_now = "25-02-2023";
    std::cout << "*******************************************initial*******************************************" << std::endl;
    h_manager.update_date(dateManager::convert(date_now));
    h_manager.print_info();

    int day_pass = 0;

    std::cout << "*****************************************pass " << day_pass << " day******************************************" << std::endl;
    date::year_month_day current = dateManager::inc_days(dateManager::convert(date_now), day_pass);
    h_manager.update_date(current);

    h_manager.print_info();


    std::cout <<"ave = " << h_manager.check_room_available(dateManager::convert("23-02-2023"),
     dateManager::convert("24-02-2023"), "301", 1) << std::endl;




    /////////////////////////////// cancel ///////////////////////////////////////
    std::cout << "*****************************************after cancelation******************************************" << std::endl;
    std::cout << "current date = " << current << std::endl;
    std::string room_num = "301";
    int user_id = 2;
    int num = 1;
    if(!h_manager.room_num_exist(room_num)){
        LOG(WARNING) << "Room doesn't exist";
    }
    else if(!h_manager.check_user_reserved(room_num, user_id) ||
                 !h_manager.cancelation_capacity_validation(room_num, user_id, num)){
        LOG(WARNING) << "User doesn't have enough reservation";
    }
    else if(!h_manager.cancelation_date_validation(current, user_id, room_num)){
        LOG(WARNING) << "User already in room. Can't cancel";
    }
    else{
        h_manager.cancel_reservation(user_id, room_num, num);
    }
    h_manager.print_info();

    ///////////////////////////// cancel  end/////////////////////////////////////


    /////////////////////////////// book ///////////////////////////////////////
    std::cout << "*****************************************after booking******************************************" << std::endl;
    std::cout << "current date = " << current << std::endl;
    room_num = "301";
    // std::string token = "dfvdsgdgdfogdg";
    // int user_id = manager.get_id(token);
    user_id = 2;
    int num_of_beds = 1;
    date::year_month_day check_in = dateManager::convert("26-02-2023");
    date::year_month_day check_out = dateManager::convert("28-02-2023");

    if(!h_manager.room_num_exist(room_num)){
        LOG(WARNING) << "Room doesn't exist";
    }
    else if(h_manager.check_user_reserved(room_num, user_id)){
        LOG(WARNING) << "User can't reserve for the room twice";
    }
    // else if(!manager.have_enough_money(token, h_manager.get_total_price(room_num, num))){
    //     LOG(WARNING) << "User doesn't have enough money";
    // }
    else if(!h_manager.booking_date_validation(current, check_in, check_out)){
        LOG(WARNING) << "Dates are invalid!";
    }
    else if(!h_manager.check_room_available(check_in, check_out, room_num, num_of_beds)){
        LOG(WARNING) << "The room is full.";
    }
    else{
        h_manager.book(room_num, user_id, num_of_beds, check_in, check_out);
    }
    h_manager.print_info();

    ///////////////////////////// book  end/////////////////////////////////////



    /////////////////////////////// leave ///////////////////////////////////////
    std::cout << "*****************************************after leaving******************************************" << std::endl;
    std::cout << "current date = " << current << std::endl;
    room_num = "301";
    user_id = 3;

    if(!h_manager.room_num_exist(room_num)){
        LOG(WARNING) << "Room doesn't exist";
    }
    else if(!h_manager.is_user_in_room(current, room_num, user_id)){
        LOG(WARNING) << "User isn't in the room";
    }
    else{
        h_manager.left_user_room(room_num, user_id);
    }
    h_manager.print_info();

    ///////////////////////////// leave  end /////////////////////////////////////



    /////////////////////////////// leave ///////////////////////////////////////
    std::cout << "*****************************************after empty******************************************" << std::endl;
    std::cout << "current date = " << current << std::endl;
    room_num = "201";

    if(!h_manager.room_num_exist(room_num)){
        LOG(WARNING) << "Room doesn't exist";
    }
    else{
        h_manager.make_room_empty(current, room_num);
    }
    h_manager.print_info();

    ///////////////////////////// leave  end /////////////////////////////////////

    

    /////////////////////////////// add room ///////////////////////////////////////
    std::cout << "**************************************after adding room***************************************" << std::endl;
    std::cout << "current date = " << current << std::endl;
    room_num = "155";
    int capacity = 5;
    int price_per_bed = 70;

    if(h_manager.room_num_exist(room_num)){
        LOG(WARNING) << "Room exist";
    }
    else{
        h_manager.add_room(room_num, capacity, price_per_bed);
    }
    h_manager.print_info();

    ///////////////////////////// add room  end /////////////////////////////////////



    /////////////////////////////// modify room ///////////////////////////////////////
    std::cout << "**************************************after modifying room***************************************" << std::endl;
    std::cout << "current date = " << current << std::endl;
    room_num = "102";
    capacity = 2;
    price_per_bed = 700;

    if(!h_manager.room_num_exist(room_num)){
        LOG(WARNING) << "Room doesn't exist";
    }
    else if(!h_manager.modify_validation(room_num, capacity)){
        LOG(WARNING) << "Can't modify room there is users in it.";
    }
    else{
        h_manager.modify_room(room_num, capacity, price_per_bed);
    }
    h_manager.print_info();

    ///////////////////////////// modify room  end /////////////////////////////////////


    /////////////////////////////// remove room ///////////////////////////////////////
    std::cout << "**************************************after removing room***************************************" << std::endl;
    std::cout << "current date = " << current << std::endl;
    room_num = "155";

    if(!h_manager.room_num_exist(room_num)){
        LOG(WARNING) << "Room doesn't exist";
    }
    else if(!h_manager.room_is_empty(room_num)){
        LOG(WARNING) << "Can't remove room there is users in it.";
    }
    else{
        h_manager.remove_room(room_num);
    }
    h_manager.print_info();

    ///////////////////////////// remove room  end /////////////////////////////////////


    /////////////////////////////// empty room ///////////////////////////////////////
    std::cout << "**************************************after empting room***************************************" << std::endl;
    std::cout << "current date = " << current << std::endl;
    room_num = "102";

    if(!h_manager.room_num_exist(room_num)){
        LOG(WARNING) << "Room doesn't exist";
    }
    else{
        h_manager.make_room_empty(current, room_num);
    }
    h_manager.print_info();

    ///////////////////////////// empty room  end /////////////////////////////////////















    sleep(10);

    using namespace date;
    std::cout << weekday{July/5/2001} << '\n';

    
    // std::istringstream in{"25-02-2023"};
    // std::ostringstream out;
    // out.fill('0');



    year_month_day x, y;
    x = dateManager::convert("25-02-2023");
    dateManager::inc_days(x, 6);
    std::cout << dateManager::get_string(x) << std::endl;
    y = dateManager::convert("26-04-2023");

    // in >> date::parse("%d-%m-%Y", x);
    // x = sys_days(x) + days{6};

    
    // out << unsigned{x.month()} << endl;
    // out << std::setw(2) << unsigned{x.day()} << "/"
    // << std::setw(2) << unsigned{x.month()}<< "/"
    // << x.year();

    // std::cout<<out.str()<<std::endl;


    sleep(10);
    google::ShutdownGoogleLogging();
    return 0;
}
