#include "infra/server.h"
#include <iostream>

int main() {
    auto c = get_server_config(DEFAULT_SERVER_PATH);
    std::cout << c.host_name << ':' << c.port << std::endl;
    return 0;
}
