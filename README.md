# Multi-threaded Chat Server with Group Communication

## Overview
This project implements a TCP-based chat server supporting multiple concurrent clients, private messaging, and group communication features. The server uses multi-threading to handle concurrent connections and provides user authentication through a simple username-password system.

## How to Run the Code

Ensure you have a C++ compiler and Make installed on your system.


### Compile the server and client:

```bash
make
```

### Start the server:

```bash
./server_grp
```

In separate terminal windows, run multiple clients:

```bash
./client_grp
```

Follow the prompts to enter username and password for each client.

### Use the following commands in the client:

- `/msg <username> <message>`: Send a private message
- `/broadcast <message>`: Send a message to all users
- `/create_group <group_name>`: Create a new group
- `/join_group <group_name>`: Join an existing group
- `/group_msg <group_name> <message>`: Send a message to a group
- `/leave_group <group_name>`: Leave a group
- `/exit`: Disconnect from the server

**Note:** Ensure the `users.txt` file is in the same directory as the server executable, containing valid username:password pairs.


## Features Implemented

### Core Functionality
- Multi-threaded server architecture supporting concurrent client connections
- User authentication using username/password stored in users.txt
- Real-time message broadcasting to all connected clients
- Private messaging between users
- Multiple Group creation and management
- Group messaging capabilities
- Concurrent client handling

### Command Support
The server responds to the following commands:
- `/msg <username> <message>`: Send private messages
- `/broadcast <message>`: Broadcast to all connected users
- `/create_group <group_name>`: Create a new chat group
- `/join_group <group_name>`: Join an existing group
- `/leave_group <group_name>`: Exit from a group
- `/group_msg <group_name> <message>`: Send message to group members
- `/exit`: Disconnect from the server

## Design Decisions

### Threading Model
We decided to open a new thread for each client connection to allow every client to send and receive messages without blocking any other client. This may cause higher memory overhead for a large number of connections but will have a **lesser response time** and greatly ease concurrent operations. This allows us easier state management where thread-local storage naturally separates client state
When a client disconnects, we remove them from all groups they've joined and clean up associated resources. This ensures that groups only contain active members and prevents resource leaks.

**Why Not Processes?:** Threads are lightweight compared to processes and share the same memory space, making it easier to manage shared resources like client lists and group data.

### Synchronization Strategy 
We use mutex locks to protect shared resources such as client lists, group memberships, and active user status. 
The server uses three main mutex locks to handle concurrent access:
1. `clients_mutex`: Protects the clients map containing socket-to-username mappings
2. `groups_mutex`: Guards the groups data structure
3. `active_users_mutex`: Protects the active users tracking system
   
**Why Synchronization?:** Without synchronization, multiple threads could access and modify shared data simultaneously, leading to inconsistent states or crashes.
This granular locking approach was chosen over a single global lock to improve concurrent performance by allowing non-conflicting operations to proceed in parallel. This prevents race conditions and ensures data integrity in a multi-threaded environment.
### Buffer Size
We set a fixed buffer size (1024 bytes) for message transmission. This decision balances between allowing reasonably sized messages and preventing excessive memory usage or potential buffer overflow attacks.
### 
## Implementation Details
### Key Functions
 - handle_client: Manages individual client connections, processes incoming messages, and routes them appropriately.
 - broadcast_message: Sends a message to all connected clients or members of a specific group.
 - send_private_message: Routes private messages between two users.
 - group_message: Handles sending messages to all members of a group.
 - remove_client_from_groups: Cleans up group memberships when a client disconnects.

### Key Components

#### User Authentication
- Passwords are stored in plaintext in users.txt (not recommended for production)
- Authentication happens at connection time before any other operations are allowed
- The system prevents multiple logins from the same username

#### Group Management
- Groups are stored in an unordered_map with group names as keys and member sets as values
- Group operations are protected by mutex to prevent race conditions
- Membership changes trigger notifications to other group members

#### Message Broadcasting
The broadcast system:
 -  Acquires necessary locks
 -  Iterates through connected clients
 -   Sends messages using send_all for reliability
 -    Handles partial sends and network errors
   
### Message Delivery
I implemented a reliable message delivery system using the `send_all` function that ensures complete message transmission even if the underlying TCP send calls only transmit partial data. This was crucial for maintaining message integrity in a chat application.

### Code Flow
__1. Server startup:__
   - Load user credentials
   - Create and bind socket
   - Start accepting connections

__2. Client connection:__
   - Accept connection
   - Spawn a new thread running handle_client
   - Authenticate user
   - Enter message processing loop

__3. Message processing:__
   - Parse command
   - Acquire necessary locks
   - Execute command
   - Broadcast/send responses
   - Release locks
__4. Client disconnection:__
   - Handle client disconnections  
   - clean up resources

## Testing

### Methodology
We conducted extensive testing to ensure the correct functionality of all implemented features:
Correctness Testing:

__1. Basic functionality Correctness testing:__
   - Verified user authentication with valid and invalid credentials
   - Tested private messaging between users
   - Checked group creation, joining, and messaging functionality
   - Concurrent client handling
   - Ensured proper handling of client disconnections

__2. Stress testing:__
   - Simulated multiple concurrent client connections (up to 50)
   - Tested rapid message sending from multiple clients
   - Large group operations
   - Verified server stability under high load

__3. Edge case testing:__
   - Network disconnections
   - Invalid commands
   - Verified behavior when attempting to join non-existent groups
   - Checked for proper cleanup of empty groups
   - Tested handling of empty messages and group names


### System Limitations
- Maximum message size: Limited by BUFFER_SIZE (1024 bytes) 
- Recommended maximum concurrent clients: 200 (thread pool limitations)
- Maximum group size: Limited by available system memory
- Group name length: Limited by BUFFER_SIZE
- Users can only send messages to active users
- A user cannot have multiple simultaneous connections
Empty group names or messages are not allowed
Chat server maintains user and group information only while active

## Challenges Faced

### Technical Challenges
__1. Message Fragmentation__
   - Problem: TCP stream could split messages
   - Solution: Implemented send_all with complete delivery guarantee

__2. Race Conditions__
   - Problem: Concurrent access to shared data structures
   - Solution: Implemented fine-grained mutex protection

__3. Resource Management__
   - Problem: Thread cleanup for disconnected clients
   - Solution: Used RAII and proper thread detachment

## Team Contribution
- Venkatesh (33%): Developed server code, message handling, and documentation
- Sai Nikhil (34%): Implemented core architecture and synchronization strategy
- Anas (33%): Created test cases, performed stress testing, and bug fixes

## Sources Referenced
- Stevens, W. R. (2003). UNIX Network Programming
- C++ Concurrency in Action by Anthony Williams
- Beej's Guide to Network Programming

## Declaration
I declare that this implementation is my original work. All sources have been properly referenced, and no code has been plagiarized.

## Feedback
Suggestions for future improvements:
1. Implement encrypted communications
2. Add persistent message history
3. Support file transfers
4. Add administrative features
