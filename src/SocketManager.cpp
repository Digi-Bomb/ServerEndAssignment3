#include "SocketManager.h"

// ----- Default ----- Operations -----
int total = 0;
SocketManager::SocketManager(int portNumber) {
    // Set port
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
}

SocketManager::~SocketManager() {
    for (std::thread& thread : this->m_clients) {
        thread.join();
    }
    close(this->m_socket);
}

// ----- Read -----

bool SocketManager::isBound() {
    return this->m_bound;
}

void log(std::string str) {
    std::cout << str << std::endl;
}

// Hidden from other files, but not a member function
void Receive(int clientSock) {
    char recvBuf[RECEIVE_SIZE] = { 0 };
    int recvResult;
    while ((recvResult = recv(clientSock, recvBuf, RECEIVE_SIZE, 0))) {
        std::cout << "Thread: " << std::this_thread::get_id() << std::endl;
        std::cout << "Data: " << recvBuf << std::endl << std::endl;
        if (recvBuf[0] =='0') {
            log("Receive done");
            break;
        }
    }
    close(clientSock);
}

int SocketManager::Check() {
    sockaddr_in clientAddr;
    socklen_t length = sizeof(clientAddr);
    int newClient;
    while ((newClient = accept(this->m_socket, (sockaddr*)&clientAddr, &length)) != -1) {
        std::thread t(Receive, newClient);
        this->m_clients.push_back(move(t));
        total++;
        std::cout << "Total: " << total << std::endl;
        if (total >= 10) {
            log("All 10 clients");
            return 0;
        }
    }
    return 1;
}

// ----- Private ----- Read -----



