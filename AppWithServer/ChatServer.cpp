#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
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

std::vector<int> clients;
std::mutex clients_mutex;

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    // Receive username
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
        close(client_socket);
        return;
    }
    std::string username(buffer);

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            std::cout << username << " disconnected.\n";
            close(client_socket);
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
            break;
        }

        std::string message = username + ": " + std::string(buffer);
        std::cout << message << std::endl;

        // Broadcast message to all clients
        std::lock_guard<std::mutex> lock(clients_mutex);
        for (int client : clients) {
            if (client != client_socket) {
                send(client, message.c_str(), message.size(), 0);
            }
        }
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

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Failed to create socket.\n";
        return -1;
    }

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Failed to bind socket.\n";
        return -1;
    }

    if (listen(server_socket, 5) < 0) {
        std::cerr << "Error listening.\n";
        return -1;
    }

    std::cout << "Server started on port " << PORT << ". Waiting for connections...\n";

    while (true) {
        sockaddr_in client_address;
        socklen_t client_len = sizeof(client_address);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_len);
        if (client_socket >= 0) {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(client_socket);
            std::cout << "New client connected.\n";
            std::thread(handle_client, client_socket).detach();
        }
    }

    close(server_socket);

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}
