// server_grp.cpp
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

#define MAX_MSG_SIZE 1024*1024
#define BUFFER_SIZE 1024
#define PORT 12345
#define USERS_FILE "users.txt"

mutex clients_mutex;
mutex groups_mutex;
mutex active_users_mutex;
// mutex client_groups_mutex;

unordered_map<int, string> clients;
unordered_map<string, string> users;
unordered_map<string, bool> active_users;
unordered_map<string, unordered_set<int>> groups; // Group Name → Set of Clients
unordered_map<int, unordered_set<string>> client_groups;  // Client Socket → Set of Group Names

void load_users() {
    ifstream file(USERS_FILE);
    string line;
    while (getline(file, line)) {
        size_t colon = line.find(':');
        if (colon != string::npos) {
            users[line.substr(0, colon)] = line.substr(colon + 1);
            active_users[line.substr(0, colon)] = false;
        }
    }
}

ssize_t send_all(int socket, const char* buffer, size_t length) {
    if(length > MAX_MSG_SIZE){
        send(socket, "Error: Message too long.", strlen("Error: Message too long."), 0);
        return -1;
    }
    size_t total_sent = 0;  // Bytes sent so far
    while (total_sent < length) {
        ssize_t bytes_sent = send(socket, buffer + total_sent, BUFFER_SIZE, 0);
        
        if (bytes_sent < 0) {
            if (errno == EINTR) {
                continue;  // Retry if interrupted
            }
            cerr << "Error sending data: " << strerror(errno) << endl;
            return -1;  // Failure
        }
        
        total_sent += bytes_sent;
    }
    return total_sent;  // Return the total bytes sent
}


void remove_client_from_groups(int client_socket) {
    if (client_groups.find(client_socket) == client_groups.end()) {
        return;  // Client is not in any group
    }
    std::lock_guard<std::mutex> lock(groups_mutex);
    // std::lock_guard<std::mutex> lock(client_groups_mutex);

    for (const string& group_name : client_groups[client_socket]) {
        groups[group_name].erase(client_socket);  // Remove client from the group
        
        // If group is empty, delete it
        if (groups[group_name].empty()) {
            groups.erase(group_name);
        }
    }

    // Remove client entry from tracking map
    client_groups.erase(client_socket);
}


void broadcast_message(const string& message, int exclude_socket = -1) {
    lock_guard<mutex> lock(clients_mutex);
    for (const auto& client : clients) {
        if (client.first != exclude_socket) {
            send_all(client.first, message.c_str(), message.size());
        }
    }
}

void send_private_message(int sender_socket, const string& recipient, const string& message) {
    lock_guard<mutex> lock(clients_mutex);
    for (const auto& client : clients) {
        if (client.second == recipient) {
            string msg = "[" + clients[sender_socket] + "]: " + message;
            send_all(client.first, msg.c_str(), msg.size());
            return;
        }
    }
    send_all(sender_socket, "Error: user not found.", strlen("Error: user not found."));
}

void group_message(int sender_socket, const string& group_name, const string& message) {
    lock_guard<mutex> lock(groups_mutex);
    if (groups.find(group_name) == groups.end()) {
        send_all(sender_socket, "Error: group does not exist.", strlen("Error: group does not exist."));
        return;
    }
    if (groups[group_name].find(sender_socket) == groups[group_name].end()) {
        send_all(sender_socket, "Error: you are not a member of this group.", strlen("Error: you are not a member of this group."));
        return;
    }

    string sender_name = clients[sender_socket];
    // string msg = "[Group " + group_name + "]: " + sender_name + ": " + message;
    string msg = "[" + sender_name + " from " + group_name + "]: " + message;
    for (int member : groups[group_name]) {
        // Skip sending the message back to the sender
        if (member != sender_socket) {
            send_all(member, msg.c_str(), msg.size());
        }
    }
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    string username;

    // Authentication
    send_all(client_socket, "Enter username: ", 16);
    memset(buffer, 0, BUFFER_SIZE);
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    username = buffer;

    send_all(client_socket, "Enter password: ", 16);
    memset(buffer, 0, BUFFER_SIZE);
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    string password = buffer;

    if (users.find(username) == users.end() || users[username] != password) {
        send_all(client_socket, "Authentication failed.", 22);
        close(client_socket);
        return;
    }else if(active_users[username] == true){
        send_all(client_socket, "Already Logged In!", strlen("Already Logged In!"));
        close(client_socket);
        return;
    }
    
    send_all(client_socket, "Welcome to the chat server!", strlen("Welcome to the chat server!"));

    {
        lock_guard<mutex> lock(clients_mutex);
        clients[client_socket] = username;
    }

    {
        lock_guard<mutex> lock(active_users_mutex);
        active_users[username] = true;
    }

    broadcast_message(username + " has joined the chat.", client_socket);

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) { // Client disconnected
            remove_client_from_groups(client_socket);  // Remove from all groups
            close(client_socket);  // Properly close the socket
            break;
        }

        string message(buffer);
        istringstream iss(message);
        string command;
        iss >> command;

        if (command == "/exit") {
            remove_client_from_groups(client_socket);  // Remove from all groups
            close(client_socket);  // Properly close the socket
            break;
        } else if (command == "/msg") {
            string recipient;
            iss >> recipient;
            if (recipient.empty()) {
                send_all(client_socket, "Usage: /msg <username> <message>", 32);
                continue;
            }
            string msg;
            getline(iss >> ws, msg); // Skip leading whitespace and get the rest
            if (msg.empty()) {
                send_all(client_socket, "Error: Message cannot be empty.", strlen("Error: Message cannot be empty."));
                continue;
            }
            send_private_message(client_socket, recipient, msg);
        }
         else if (command == "/broadcast") {
            string msg;
            getline(iss, msg);
            if (msg.empty()) {
                send_all(client_socket, "Error: Message cannot be empty.", strlen("Error: Message cannot be empty."));
                continue;
            }
            msg = msg.substr(1); // Remove leading space
            // broadcast_message("[Broadcast] " + username + ": " + msg, client_socket);
            broadcast_message("[Broadcast from " + username + "]: " + msg, client_socket);
        } else if (command == "/create_group") {
            string group_name;
            iss >> group_name;
            if (group_name.empty()) {
                send_all(client_socket, "Error: Group name cannot be empty", strlen("Error: Group name cannot be empty"));
                continue;
            }
            lock_guard<mutex> lock(groups_mutex);
            if (groups.find(group_name) == groups.end()) {
                groups[group_name].insert(client_socket);
                send_all(client_socket, ("Group \"" + group_name + "\" created.").c_str(), ("Group \"" + group_name + "\" created.").size());
            } else {
                send_all(client_socket, "Error: Group already exists.", strlen("Error: Group already exists."));
            }
        } else if (command == "/join_group") {
            std::string group_name;
            iss >> group_name;
            if (group_name.empty()) {
                send_all(client_socket, "Error: Group name cannot be empty", strlen("Error: Group name cannot be empty"));
                continue;
            }
            std::lock_guard<std::mutex> lock(groups_mutex);
            // std::lock_guard<std::mutex> lock(client_groups_mutex);
            if (groups.find(group_name) != groups.end()) {
                // Check if already in group
                if (groups[group_name].find(client_socket) != groups[group_name].end()) {
                    send_all(client_socket, "You are already in this group.", 31);
                } else {
                    groups[group_name].insert(client_socket);
                    client_groups[client_socket].insert(group_name);
                    send_all(client_socket, ("You joined the group " + group_name + ".").c_str(), ("You joined the group " + group_name + ".").size());

                    // Notify group members
                    std::string username;
                    {
                        std::lock_guard<std::mutex> clock(clients_mutex);
                        auto it = clients.find(client_socket);
                        if (it != clients.end()) username = it->second;
                    }
                    if (!username.empty()) {
                        std::string msg = username + " has joined the group " + group_name + ".";
                        for (int member : groups[group_name]) {
                            if (member != client_socket) {
                                send_all(member, msg.c_str(), msg.size());
                            }
                        }
                    }
                }
            } else {
                send_all(client_socket, "Error: Group does not exist.", strlen("Error: Group does not exist."));
            }
        } else if (command == "/leave_group") {
            std::string group_name;
            iss >> group_name;
            if (group_name.empty()) {
                send_all(client_socket, "Error: Group name cannot be empty", strlen("Error: Group name cannot be empty"));
                continue;
            }
            std::lock_guard<std::mutex> lock(groups_mutex);
            if (groups.find(group_name) != groups.end()) {
                if (groups[group_name].erase(client_socket)) {
                    send_all(client_socket, ("You left the group " + group_name + ".").c_str(), ("You left the group " + group_name + ".").size());

                    // Notify group members
                    std::string username;
                    {
                        std::lock_guard<std::mutex> clock(clients_mutex);
                        auto it = clients.find(client_socket);
                        if (it != clients.end()) username = it->second;
                    }
                    if (!username.empty()) {
                        std::string msg = username + " has left the group " + group_name + ".";
                        for (int member : groups[group_name]) {
                            send_all(member, msg.c_str(), msg.size());
                        }
                    }
                } else {
                    send_all(client_socket, "Error: You are not in this group.", strlen("Error: You are not in this group."));
                }
            } else {
                send_all(client_socket, "Error: Group does not exist.", strlen("Error: Group does not exist."));
            }
        } else if (command == "/group_msg") {
            string group_name;
            iss >> group_name;
            if (group_name.empty()) {
                send_all(client_socket, "Usage: /group_msg <group_name> <message>", 39);
                continue;
            }
            string msg;
            getline(iss >> ws, msg); // Skip leading whitespace
            if (msg.empty()) {
                send_all(client_socket, "Error: Message cannot be empty.", 24);
                continue;
            }
            group_message(client_socket, group_name, msg);
        } else {
            const char* error_msg = "Error: Invalid Command.";
            send_all(client_socket, error_msg, strlen(error_msg));
        }
    }

    {
        lock_guard<mutex> lock(clients_mutex);
        clients.erase(client_socket);
    }

    {
        lock_guard<mutex> lock(active_users_mutex);
        active_users[username]=false;
    }
    close(client_socket);
    broadcast_message(username + " has left the chat.", client_socket);
}

int main() {
    load_users();   // Load users from userts.txt file into the users map

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        cerr << "Error: Socket failed";
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        cerr << "setsockopt failed";
        return 1;
    }

    // Bind the socket to the address
    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Error: Bind failed."<<endl;
        return 1;
    }
    // Listen for incoming connections
    if (listen(server_socket, 5) < 0) {
        cerr << "Error: Listen failed.";
        return 1;
    }

    cout << "Server listening on port " << PORT << "...." << endl;

    while (true) {
        sockaddr_in client_addr{};
        socklen_t addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (sockaddr*)&client_addr, &addr_len);
        if (client_socket < 0) {
            cerr << "Error: Accept failed.";
            continue;
        }

        thread t(handle_client, client_socket);
        t.detach();
    }

    close(server_socket);
    return 0;
}
