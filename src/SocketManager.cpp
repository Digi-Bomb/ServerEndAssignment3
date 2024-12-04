#include "SocketManager.h"

#include <mutex>
#include "FileManager.h"

// ----- Default ----- Operations -----

SocketManager::SocketManager(int portNumber) {
    // Set port
    this->m_close = false;
    if (0 < portNumber && portNumber <= 65535) {
        this->m_port = portNumber;
    }
    else {
        this->m_port = DEFAULT_PORT;
    }

    // Create socket
    this->m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (this->m_socket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        this->m_bound = false;
        return;
    }

    // Bind socket
    this->m_addr.sin_family = AF_INET;
    this->m_addr.sin_addr.s_addr = INADDR_ANY;
    this->m_addr.sin_port = htons(this->m_port);
    if (bind(this->m_socket, (sockaddr*)&this->m_addr, sizeof(this->m_addr)) == -1) {
        std::cerr << "Failed to bind socket" << std::endl;
        this->m_bound = false;
        return;
    }

    // Begin listen
    if (listen(this->m_socket, SOMAXCONN) == -1) {
        std::cerr << "Failed to listen" << std::endl;
        this->m_bound = false;
        return;
    }

    // Everything went properly
    this->m_bound = true;

    // Read data from file
    auto data = FileManager::Read(FILENAME);
    for (std::string& str : data) {
        this->m_posts.push_back(str);
    }
}

SocketManager::~SocketManager() {
    // Close threads
    for (std::thread& thread : this->m_clients) {
        thread.join();
    }
    close(this->m_socket);

    // Write to file
    FileManager::Write(FILENAME, this->m_posts);
}

// ----- Read -----

bool SocketManager::isBound() {
    return this->m_bound;
}

// Hidden from other files, but not a member function
void SocketManager::Receive(int clientSocket) {
    char recvBuf[RECEIVE_SIZE] = { 0 };
    int recvResult;
    while ((recvResult = recv(clientSocket, recvBuf, RECEIVE_SIZE, 0))) {
        std::string received = recvBuf;
        if (received.compare("[exit]") == 0) {
            this->m_close = true;
            break;
        }
        this->Add(received);
    }
    close(clientSocket);
}

int SocketManager::Check() {
    sockaddr_in clientAddr;
    socklen_t length = sizeof(clientAddr);
    int newClient;
    while ((newClient = accept(this->m_socket, (sockaddr*)&clientAddr, &length)) != -1) {
        std::thread t(&SocketManager::Receive, this, newClient);
        this->m_clients.push_back(move(t));
        if (this->m_close) {
            break;
        }
    }
    return 0;
}

// ----- Update -----

void SocketManager::Add(const std::string& post) {
    std::lock_guard<std::mutex> guard(std::mutex);
    this->m_posts.push_back(post);
}

