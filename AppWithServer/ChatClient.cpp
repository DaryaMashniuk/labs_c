#include <iostream>
#include <thread>
#include <string>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

#ifdef _WIN32
#define close closesocket
#endif

void receive_messages(int socket) {
    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            std::cerr << "Connection to the server lost.\n";
            close(socket);
            exit(0);
        }
        std::cout << buffer << std::endl;
    }
}

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Winsock initialization error\n";
        return 1;
    }
#endif

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Failed to create socket.\n";
        return -1;
    }

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Failed to connect to the server.\n";
        return -1;
    }

    std::cout << "Connected to the server.\n";

    // Prompt for username
    std::string username;
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);

    // Send username to the server
    send(client_socket, username.c_str(), username.size(), 0);

    // Start receiving messages
    std::thread(receive_messages, client_socket).detach();

    // Main loop for sending messages
    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (message.empty()) continue;
        // Send only the message, not the username
        send(client_socket, message.c_str(), message.size(), 0);
    }

    close(client_socket);

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}