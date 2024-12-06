#include "SocketManager.h"

#include <mutex>
#include <poll.h>
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
        std::cerr << "Failed to create socket\n";
        this->m_bound = false;
        return;
    }

    // Bind socket
    this->m_addr.sin_family = AF_INET;
    this->m_addr.sin_addr.s_addr = INADDR_ANY;
    this->m_addr.sin_port = htons(this->m_port);
    if (bind(this->m_socket, (sockaddr*)&this->m_addr, sizeof(this->m_addr)) == -1) {
        std::cerr << "Failed to bind socket\n";
        this->m_bound = false;
        return;
    }

    std::cout << "Server bound to port " << this->m_port << "\n";

    // Begin listen
    if (listen(this->m_socket, SOMAXCONN) == -1) {
        std::cerr << "Failed to listen\n";
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

    // Accept thread is detached
    this->m_acceptThread = std::thread(&SocketManager::Check, this);
}

SocketManager::~SocketManager() {
    // Close threads
    for (std::thread& thread : this->m_clients) {
        thread.join();
    }

    // Close socket
    if (this->m_socket != -1) {
        close(this->m_socket);
    }

    if (!this->m_bound) {
        return;
    }

    // Join accept thread
    this->m_acceptThread.join();

    // Write to file
    FileManager::Write(FILENAME, this->m_posts);
}

// ----- Read -----

bool SocketManager::isBound() {
    return this->m_bound;
}

bool SocketManager::ShouldClose() {
    return this->m_close;
}

// Hidden from other files, but not a member function
void SocketManager::Receive(int clientSocket) {
    std::cout << "Client connected on thread: " << std::this_thread::get_id() << "\n";
    char recvBuf[RECEIVE_SIZE] = { 0 };
    int recvResult;
    while ((recvResult = recv(clientSocket, recvBuf, RECEIVE_SIZE, 0))) {
        std::string received = recvBuf;
        if (this->Parse(received, clientSocket) == PARSE_EXIT) {
            break;
        }
        if (received.compare("[exit]") == 0) {
            this->m_close = true;
            std::cout << "Client demanding server be shutdown\n";
            break;
        }
    }
    close(clientSocket);
}

int SocketManager::Parse(std::string received, int clientSocket) {
    std::cout << "Received: " << received << std::endl;
    std::cout << "Detected: ";
    if (received.substr(0,6).compare("[post]") == 0) {
        std::cout << "Post\n";
        this->Add(received);
        return PARSE_POST;
    }

    else if (received.substr(0,12).compare("[requestall]") == 0) {
        std::cout << "Request All\n";

        //Send vector data
        std::string data;
        for (std::string& d : this->m_posts) {
            data += d;
        }
        std::cout << "Sending: " << data.c_str() << "\n";
        if (send(clientSocket, data.c_str(), data.length(), 0) == -1) {
            std::cerr << "Message Send Failure\n";
            return PARSE_ERROR;
        }
        return PARSE_REQA;
    }

    else if (received.substr(0,7).compare("[close]") == 0) {
        std::cout << "Client Exiting\n";
        return PARSE_EXIT;
    }

    return PARSE_ERROR;
}

void SocketManager::Check() {
    std::cout << "Server checking connections on thread " << std::this_thread::get_id() << "\n";
    // Checks for client connections
    sockaddr_in clientAddr;
    socklen_t length = sizeof(clientAddr);
    int newClient;
    struct pollfd fds[1];
    fds[0].fd = this->m_socket;
    fds[0].events = POLLIN | POLLPRI;

    while (!this->ShouldClose()) {
        // Poll socket for 3000ms
        if (poll(fds, 1, 3000)) {
            if ((newClient = accept(this->m_socket, (sockaddr*)&clientAddr, &length)) != -1) {
                std::thread t(&SocketManager::Receive, this, newClient);
                this->m_clients.push_back(move(t));
            }
        }
    }
}

// ----- Update -----

void SocketManager::Add(const std::string& post) {
    std::lock_guard<std::mutex> guard(std::mutex);
    this->m_posts.push_back(post);
    FileManager::Write(FILENAME, this->m_posts);
}

