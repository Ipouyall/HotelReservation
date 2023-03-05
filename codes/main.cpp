#include "infra/server.h"
#include "infra/hotel.h"
#include "infra/user.h"
#include <iostream>
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
        cout << _h.room_number << ' ' << _h.max_capacity - _h.current_capacity << endl;
    }

    auto u = get_users_data(DEFAULT_USERS_PATH);
    for (auto _u:u){
        cout << _u.id << ' ' << (_u.privilege ? _u.username : _u.phone_number) << endl;
    }
    sleep(10);

    google::ShutdownGoogleLogging();
    return 0;
}
