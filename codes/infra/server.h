//
// Created by Pooya Sadeghi on 2023-02-28.
//

#ifndef SRC_SERVER_H
#define SRC_SERVER_H

#include "filehandler.h"

#include <string>

using namespace std;

#define DEFAULT_SERVER_PATH "./config/config.json"

typedef struct serverConfig {
    string host_name;
    int port;
} serverConfig;

serverConfig get_server_config(string path);

#endif //SRC_SERVER_H
