#include "server.h"
#include "types/constants.h"
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    std::cout.setf(std::ios::unitbuf);

    uint16_t port = tao::constants::DEFAULT_PORT;
    if (argc > 1) port = static_cast<uint16_t>(std::atoi(argv[1]));

    std::cout << "Tao Arkanoid Server" << std::endl;
    tao::Server server;
    if (!server.start(port)) return 1;
    server.run();
    return 0;
}
