# Multi-threaded Chat Server with Group Communication

## Project Description
This project implements a TCP-based chat server supporting multiple concurrent clients, message transfer(broadcast/unicast), and group communication features. The server uses multi-threading to handle concurrent connections and provides user authentication through a simple username-password system. 

## Team Members
Venkatesh Akula (220109)

Mohammed Anas (220654)

Sai Nikhil (221095)

## File structure
```bash

chat/
├── Makefile  (Compilation script)
├── users.txt (users details)
├── README.md  (Project Report)
├── Assets (images & gifs)
├── client_grp.cpp (Client source code)
└── server_grp.cpp (Server source code)
```

## Instructions to Run the Code

Ensure you have a linux/MacOS system with C++ compiler and Make installed on your system.


### Compile the server and client:

```bash
make
```

### Run the server:

```bash
./server_grp
```

### Run the client:

```bash
./client_grp
```

In separate terminal windows, you can run multiple clients.

When prompted, enter the **username** and **password** for each client present in **'users.txt'**

### Use the following commands in the client:

- `/msg <username> <message>`: Send a private message
- `/broadcast <message>`: Send a message to all users
- `/create_group <group_name>`: Create a new group
- `/join_group <group_name>`: Join an existing group
- `/group_msg <group_name> <message>`: Send a message to a group
- `/leave_group <group_name>`: Leave a group
- `/exit`: Disconnect from the server

> **Note:**  Ensure the `users.txt` file is in the same directory as the server executable, containing valid username:password pairs.


## Features Implemented

We have implemented **all the features** described in the assignment. Some important functionalities are mentioned below.

### <ins>Core Functionality</ins>
- Multi-threaded server architecture supporting concurrent client connections
- User authentication using username/password stored in users.txt
- Real-time message broadcasting to all connected clients
- Private messaging Group messaging capabilities between users
- Multiple Group creation and management
- Concurrent client handling

### <ins>Command Support</ins>
The server responds to the following commands:
- `/msg <username> <message>`: Send private messages
- `/broadcast <message>`: Broadcast to all connected users
- `/create_group <group_name>`: Create a new chat group
- `/join_group <group_name>`: Join an existing group
- `/leave_group <group_name>`: Exit from a group
- `/group_msg <group_name> <message>`: Send message to group members
- `/exit`: Disconnect from the server

## Design Decisions

### <ins>Threading Model<ins>
We decided to open a new thread for each client connection to allow every client to send and receive messages without blocking any other client. This may cause higher memory overhead for a large number of connections but will have a **lesser response time** and greatly ease concurrent operations. This allows us easier state management where thread-local storage naturally separates client state
When a client disconnects, we remove them from all groups they've joined and clean up associated resources. This ensures that groups only contain active members and prevents resource leaks.

>**Why Not Processes?:** Threads are lightweight compared to processes and share the same memory space, making it easier to manage shared resources like client lists and group data.

### <ins>Synchronization Strategy</ins>
We use mutex locks to protect shared resources such as client lists, group memberships, and active user status. 
The server uses three main mutex locks to handle concurrent access:
1. `clients_mutex`: Protects the clients map containing socket-to-username mappings
2. `groups_mutex`: Guards the groups data structure
3. `active_users_mutex`: Protects the active users tracking system
   
>**Why Synchronization?:** Without synchronization, multiple threads could access and modify shared data simultaneously, leading to inconsistent states or crashes.
This granular locking approach was chosen over a single global lock to improve concurrent performance by allowing non-conflicting operations to proceed in parallel. This prevents race conditions and ensures data integrity in a multi-threaded environment.

### <ins>Message Handling</ins>
We set a fixed buffer size (1MB) for message transmission. This decision balances between allowing reasonably sized messages and **preventing** excessive memory usage or potential **buffer overflow attacks.**

### <ins>Empty group handle</ins>
We have decided to **remove** all the empty groups dynamically whenever they get created by taking inspiration from **Whatsapp**. 
Also, the server does not allow empty messages and group names.

### <ins>Single Connection Per User</ins>
The server enforces single connection per user. We have implemented a check in the authentication process prevents multiple simultaneous connections by maintaining an active_users map.

### <ins>Active users management </ins>
The chat server only maintains active users' information and the sockets assigned to them. Once a user leaves the group, all his data and the sockets assigned to them will be cleaned.


### 
## Implementation Details

![Overview](/Assets/Chat-Server.gif)

### Key Functions
 1. `handle_client`: Manages individual client connections, processes incoming messages, and routes them appropriately.
 2. `broadcast_message`: Sends a message to all connected clients or members of a specific group.
 3. `send_private_message`: Routes private messages between two users.
 4. `group_message`: Handles sending messages to all members of a group.
 5. `remove_client_from_groups`: Cleans up group memberships when a client disconnects.

### Key Components

#### <ins>User Authentication</ins>
- Passwords are stored in plaintext file in **users.txt**
- Authentication happens at connection time before any other operations are allowed
- The system **prevents multiple logins** from the same username

#### <ins>Group Management</ins>
- Groups are stored in an unordered_map with group names as keys and member sets as values
- Group operations are protected by mutex to prevent race conditions
- Group leaving/joining changes trigger notifications to other group members

#### <ins>Message Broadcasting/Unicasting</ins>
The broadcast system:
 -  Acquires necessary locks
 -  Iterates through connected clients
 -  Sends messages using send_all for reliability
 -  Handles partial sends and network errors
   

### Chat Server Code Flow

#### 1. <ins>Server Startup Process</ins>

##### Initialization Steps
- Loads user credentials from `users.txt`
- Creates a TCP socket
- Configures socket options
- Binds to network address
- Starts listening for connections

##### Key Initialization Code
```cpp
    load_users();  // Load user credentials
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_socket, 5);  // Queue up to 5 pending connections
```

#### 2. <ins>Client Connection Workflow</ins>

##### Connection Acceptance
- Continuously listens for incoming client connections
- When a connection is received, creates a new thread
- Each thread runs `handle_client()` function independently

```cpp
while (true) {
    int client_socket = accept(server_socket, ...);
    thread t(handle_client, client_socket);
    t.detach();  // Allow thread to run independently
}
```

#### 3. <ins>Authentication Process</ins>

##### User Verification Steps
1. Prompt for username
2. Prompt for password
3. Validate against stored credentials
4. Check for existing active session
5. Reject if authentication fails

```cpp
// Authentication validation
if (users.find(username) == users.end() || 
    users[username] != password) {
    // Send authentication failure message
    close(client_socket);
    return;
}else if(active_users[username] == true){
    // Send already logged in message
    close(client_socket);
    return;
}
```

#### 4. <ins>Message Processing Loop</ins>

##### Command Handling Mechanism
- Receives client messages
- Parses command
- Applies appropriate action
- Uses mutex locks for thread safety

```cpp
while (true) {
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    istringstream iss(message);
    string command;
    iss >> command;
    // Command processing switch-like logic
    if (command == "/msg") {
        send_private_message(client_socket, recipient, message);
    } else if (command == "/broadcast") {
        broadcast_message(message);
    }
    // ... other command handlers
}
```

#### 5. <ins>Client Disconnection Handling</ins>

##### Resource Cleanup
- Remove client from active connections
- Clear group memberships
- Update active user status
- Close socket
- Notify other users

```cpp
// Disconnection process
remove_client_from_groups(client_socket);
clients.erase(client_socket);
active_users[username] = false;
close(client_socket);
broadcast_message(username + " has left the chat.");
```

#### 6. <ins>Concurrency and Thread Safety</ins>

##### Synchronization Mechanisms
- `mutex` for protecting shared resources
- `lock_guard` for automatic lock management
- Separate mutexes for:
  - Client list
  - Group management
  - Active user tracking

```cpp
// Example of thread-safe operation
{
    lock_guard<mutex> lock(clients_mutex);
    clients[client_socket] = username;
}
```

#### 7. <ins>Error Handling Approaches</ins>

##### Error Management
- Validate user inputs
- Provide meaningful error messages
- Gracefully handle network interruptions
- Prevent unauthorized actions

```cpp
// Error handling example
if (group_name.empty()) {
    send_all(client_socket, 
             "Error: Group name cannot be empty", 
             strlen("Error: Group name cannot be empty"));
}
```

#### 8. <ins>Security Considerations</ins>

##### Basic Security Measures
- Prevent multiple logins
- Simple username/password authentication
- Message size limits
- Socket error handling


## Testing


### <ins>Methodology</ins>
We conducted extensive testing to ensure the correct functionality of all implemented features:
Correctness Testing:

__1. <ins>Basic functionality Correctness testing:</ins>__
   - Verified user authentication with valid and invalid credentials
   - Tested private messaging between users
   - Checked group creation, joining, and messaging functionality
   - Concurrent client handling
   - Ensured proper handling of client disconnections
Please find the image below illustrating an interaction of 5 clients with the server.
![Overview](/Assets/correctness_testing.png)

__2. <ins>Stress testing:</ins>__
  - Connection Handling: Successfully tested with 1000 simultaneous connections
  - Message Broadcasting: Handled 850 messages/second across 50 active clients. Tested rapid message sending from multiple clients
  - Group Operations: Maintained stability with 20 groups having 50 members each ensuring server stability under high load
   
| Parameter          | Maximum Limit found    | Explaination   |
|--------------------|-------------------|--------------------------------------|
| Concurrent Clients  | 1000              | Refer the images    |
| Active Groups      | Unlimited         | Bounded by system memory   |
| Group Members      | Unlimited         | Bounded by concurrent clients |
| Message Size       | 1MB               | Defined by MAX_MSG_SIZE (1024*1024 bytes) |

![Overview](/Assets/stress_testing1.jpeg)
**Code used for stress testing**
```
import subprocess
import time
import threading

def read_users_from_file(filename):
    users = []
    with open(filename, 'r') as file:
        for line in file:
            username, password = line.strip().split(':')
            users.append((username, password))
    return users

def run_client(username, password):
    subprocess.Popen(["./client_grp"], stdin=subprocess.PIPE, text=True).communicate(
        input=f"{username}\n{password}\n"
    )
    print(f"User {username} connected")

def main():
    users = read_users_from_file("users.txt")
    threads = []

    for username, password in users:
        thread = threading.Thread(target=run_client, args=(username, password))
        threads.append(thread)

    # Start all threads
    for thread in threads:
        thread.start()
        time.sleep(0.1)  # Small delay to stagger connections

    # Wait for all threads to complete
    for thread in threads:
        thread.join()

if __name__ == "__main__":
    main()
```
![Overview](/Assets/stress_testing2.jpeg)


__3. <ins>Edge case testing:</ins>__
   - Network disconnections
   - Invalid commands
   - Verified behavior when attempting to join non-existent groups
   - Checked for proper cleanup of empty groups
   - Tested handling of empty messages and group names


### System Limitations
- Maximum message size: Limited by MAX_MSG_SIZE (1 mega byte) 
- Recommended maximum concurrent clients: 1000 
- Maximum group size: Limited by available system memory
- Group name length: Limited by BUFFER_SIZE

## Challenges Faced

### Technical Challenges
__1. Message Fragmentation__
   - **Problem**: TCP stream could split messages
   - **Solution**: Implemented send_all and kept a threshold for message size
     
__2. Race Conditions__
   - **Problem**: Concurrent access to shared data structures
   - **Solution**: Implemented fine-grained mutex protection

__3. Resource Management__
   - **Problem**: Thread cleanup for disconnected clients
   - **Solution**: Once the client is disconnected, all resources allocated to him are removed and proper thread detachment done
     
__4. Binding issue__
   - **Problem**: Immediate login after disconnection caused binding error
   - **Solution**: It is due to port not being released quickly DO-REUSEADDR option is used and properly handled
     
## Team Contribution
- Venkatesh (33.33%): Developed server code, message handling, and documentation
- Sai Nikhil (33.33%): Implemented core architecture and performed stress testing
- Anas (33.33%): Created test cases, synchronization strategy, and bug fixes

## Sources Referenced
- Stevens, W. R. (2003). UNIX Network Programming
- C++ Concurrency in Action by Anthony Williams
- Beej's Guide to Network Programming

## Declaration
I declare that this implementation is our original work. All sources have been properly referenced, and no code has been plagiarized.

## Feedback
Suggestions for future improvements:
1. Implement encrypted communications
2. Add persistent message history
3. Support file transfers
4. Add administrative features
