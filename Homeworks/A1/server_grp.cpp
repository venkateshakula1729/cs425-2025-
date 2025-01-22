// Client-side implementation in C++ for a chat server with private messages and group messaging

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

std::mutex cout_mutex;

void handle_server_messages(int server_socket) {
    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(server_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "Disconnected from server." << std::endl;
            close(server_socket);
            exit(0);
        }
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << buffer << std::endl;
    }
}

int main() {
    int server_socket;
    sockaddr_in server_address{};

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        std::cerr << "Error creating socket." << std::endl;
        return 1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(12345);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // if (connect(server_socket, (sockaddr*)&server_address, sizeof(server_address)) < 0) {
    //     std::cerr << "Error connecting to server." << std::endl;
    //     return 1;
    // }
    int clientSocket
        = accept(server_socket, nullptr, nullptr);

    std::cout << "Connected to the server." << std::endl;

    // Authentication
    // std::string username, password;
    // char username[BUFFER_SIZE], password[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    std::strcpy(buffer,"Enter the username");

    // memset(buffer, 0, BUFFER_SIZE);
    send(clientSocket, buffer, BUFFER_SIZE, 0); // Receive the message "Enter the user name" for the server
    // You should have a line like this in the server.cpp code: send_message(client_socket, "Enter username: ");
 
    // std::cout << buffer;\
    // std::getline(std::cin, username);
    // char buffer[BUFFER_SIZE];

    recv(clientSocket, buffer, BUFFER_SIZE, 0);
    std::cout<<buffer<<std::endl;
    // char username[]=buffer;
    std::string username(buffer);
    // std::strcpy(username, buffer);
    // memset(buffer, 0, BUFFER_SIZE);
    // char buffer[]="Enter the password";
    std::strcpy(buffer,"Enter the password");
    send(clientSocket, buffer, BUFFER_SIZE, 0); // Receive the message "Enter the password" for the server
    // std::cout << buffer;
    // std::getline(std::cin, password);
    memset(buffer, 0, BUFFER_SIZE);
    recv(clientSocket, buffer, BUFFER_SIZE, 0);
    std::cout<<buffer<<std::endl;
    std::string password(buffer);
    // std::strcpy(password, buffer);
    memset(buffer, 0, BUFFER_SIZE);
    int check=1;
    // password check code here
    if(check)
    {
        std::strcpy(buffer,"Welcome to the server");
        send(clientSocket, buffer, BUFFER_SIZE, 0);
    }
    else
    {
        std::strcpy(buffer,"Authentication Failed");
        send(clientSocket, buffer, BUFFER_SIZE, 0);
    }
    // Depending on whether the authentication passes or not, receive the message "Authentication Failed" or "Welcome to the server"
    // recv(server_socket, buffer, BUFFER_SIZE, 0); 
    // std::cout << buffer << std::endl;

    // if (std::string(buffer).find("Authentication failed") != std::string::npos) {
    //     close(server_socket);
    //     return 1;
    // }

    // // Start thread for receiving messages from server
    // std::thread receive_thread(handle_server_messages, server_socket);
    // // We use detach because we want this thread to run in the background while the main thread continues running
    // receive_thread.detach();

    // Send messages to the server
    // while (true) {
    //     std::string message;
    //     std::getline(std::cin, message);

    //     if (message.empty()) continue;

    //     send(server_socket, message.c_str(), message.size(), 0);

    //     if (message == "/exit") {
    //         close(server_socket);
    //         break;
    //     }
    // }

    return 0;
}
