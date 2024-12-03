#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <thread>

#define DEFAULT_PORT    27000
#define RECEIVE_SIZE    1024

class SocketManager {
private:
    // Port the manager is attached to
    int m_port, m_socket;
    // Server address info
    sockaddr_in m_addr;
    // For checking if the manager was successfully bound to the port
    bool m_bound;

    std::vector<std::thread> m_clients;
    
public:
    // Create
    SocketManager(int portNumber);

    // Read
    bool isBound();
    int Check();

    // Update


    // Destroy
    ~SocketManager();
};
