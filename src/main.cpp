#include <iostream>

#include "SocketManager.h"

int main(void) {
    // Create and check socketmanager for managing connections
    SocketManager server(DEFAULT_PORT);
    if (!server.isBound()) {
        std::cerr << "Something went wrong" << std::endl;
        return 0;
    }

    // Loop while the server should be running
    while (!server.ShouldClose()) {}
    
    return 0;
}
