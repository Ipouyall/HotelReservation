//
// Created by Pooya Sadeghi on 2023-02-28.
//

#include "server.h"
#include <iostream>

serverConfig get_server_config(string path) {
    auto conf = serverConfig{};

    auto j = readJsonFile(path);
    if (j == nullptr)
        return conf;

    conf.host_name = j["hostName"];
    conf.port = j["commandChannelPort"];

    return conf;
}
