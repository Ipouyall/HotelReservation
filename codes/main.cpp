#include "infra/server.h"
#include "infra/hotel.h"
#include <iostream>
#include <glog/logging.h>

void signalHandler(int signum) {
    LOG(INFO) << "Received signal " << signum << ", terminating program.";
    google::ShutdownGoogleLogging();
    exit(signum);
}

int main() {
    google::InitGoogleLogging("Server");
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    FLAGS_colorlogtostderr = true;
    FLAGS_log_prefix = true;
    FLAGS_logtostderr = true;
    FLAGS_alsologtostderr = true;

    LOG(INFO) << "Initializing program ...";

    auto c = get_server_config(DEFAULT_SERVER_PATH);
    std::cout << c.host_name << ':' << c.port << std::endl;


    auto h = get_hotel_information(DEFAULT_HOTEL_PATH);

    google::ShutdownGoogleLogging();
    return 0;
}
