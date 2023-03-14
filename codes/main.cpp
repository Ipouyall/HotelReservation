#include "infra/server.h"
#include "infra/hotel.h"
#include "infra/user.h"
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <glog/logging.h>

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

    std::cout << "before insertion" << std::endl;
    manager.printINfo();

    manager.signup("sfdgdfgffd", "pokopkp54", 50, "0912235222", "werw street, ookok valley");
    std::string token = manager.login("sfdgdfgffd", 4);
    manager.reduce_balance(token, 20);

    std::cout << "after insertion" << std::endl;

    manager.printINfo();




    sleep(10);

    google::ShutdownGoogleLogging();
    return 0;
}
