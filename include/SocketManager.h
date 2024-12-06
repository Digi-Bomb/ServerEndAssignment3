#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <thread>

#define DEFAULT_PORT    27000
#define RECEIVE_SIZE    1024
#define FILENAME        "PostData.txt"

#define PARSE_EXIT      0x01
#define PARSE_REQA      0x02
#define PARSE_POST      0x03
#define PARSE_ERROR     0x04

class SocketManager {
private:
    // Port the manager is attached to
    int m_port, m_socket;
    // Server address info
    sockaddr_in m_addr;
    // For checking if the manager was successfully bound to the port
    bool m_bound, m_close;

    // Thread for accepting new clients
    std::thread m_acceptThread;
    // All the client threads
    std::vector<std::thread> m_clients;

    std::vector<std::string> m_posts;
    
    // Once a connection is established, receive messages
    void Receive(int clientSocket);
    
    // Checks for connections on separate thread
    void Check();
    
    // Parse the received string
    int Parse(std::string received, int clientSocket);
    
public:
    // Create
    SocketManager(int portNumber);

    // Read
    bool isBound();
    bool ShouldClose();

    // Update
    void Add(const std::string& post);

    // Destroy
    ~SocketManager();
};
