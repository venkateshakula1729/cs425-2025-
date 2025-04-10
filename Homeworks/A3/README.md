# TCP Three-Way Handshake (Client-Side) Using Raw Sockets

## Introduction

The project simulates the **TCP three-way handshake** process using raw sockets in C++. The client-side logic is implemented in the `client.cpp` file, which sends a SYN packet to the server, waits for an ACK response, and finally sends an ACK packet to the server to complete the handshake.

The client is implemented with the help of raw sockets, thereby communicating with its counterpart in `server.cpp` to achieve low-level control over the entire TCP/IP stack. The goal of this project is to understand the complex process of TCP at the packet level.

**Objective:**
1. Send an **SYN** packet to the server, specifying a particular sequence number.
2. Receive and validate a **SYN-ACK** response from the server.
3. Send a final **ACK** packet to complete the handshake.
---

## Team Members
- Venkatesh Akula (220109)
- Mohammed Anas (220654)
- Sai Nikhil (221095)

---

## Features

- **Raw Socket Communication**: Craft and send custom TCP packets over raw sockets.
- **SYN Packet Generation**: Send SYN packet for initiating handshake.
- **SYN-ACK Handling**: Receives SYN-ACK response from the server and its processing.
- **ACK Packet Generation**: Sends ACK packet to successfully conclude handshake.
- **Packet Debugging**: Detailed info will be print when TCP packets are sent and received.

---

## Prerequisites

1. A system running Linux, as raw sockets require administrative privileges that only Linux can provide.
2. A C++ compiler, e. g., `g++`.
3. Root privileges for running the program (needed for raw socket operations).

---

## Instructions for Running the Code

### Step 1: Compile the Code

Use the following command to compile `server.cpp`, `client.cpp` files:

```sh
g++ client.cpp -o client
```

```sh
g++ server.cpp -o server
```

### Step 2: Run the Server

Ensure the server (`server.cpp`) is running before starting the client. Run the server program with root privileges:

```sh
sudo ./server
```

### Step 3: Run the Client

Open other terminal and run the client program with root privileges:

```sh
sudo ./client
```

### Expected Output

The client will print debug information about the packets being sent and received during the handshake process. For example:

```
[+] Sending SYN to server...
[SENT SYN] Packet Info:
    Source IP: 127.0.0.1
    Dest IP:   127.0.0.1
    Source Port: 54321
    Dest Port:   12345
    Seq: 200 Ack: 0
    Flags -> SYN: 1 ACK: 0 FIN: 0 RST: 0 PSH: 0
[RECV SYN-ACK] Packet Info:
    Source IP: 127.0.0.1
    Dest IP:   127.0.0.1
    Source Port: 12345
    Dest Port:   54321
    Seq: 400 Ack: 201
    Flags -> SYN: 1 ACK: 1 FIN: 0 RST: 0 PSH: 0
[SENT ACK] Packet Info:
    Source IP: 127.0.0.1
    Dest IP:   127.0.0.1
    Source Port: 54321
    Dest Port:   12345
    Seq: 600 Ack: 401
    Flags -> SYN: 0 ACK: 1 FIN: 0 RST: 0 PSH: 0
[+] Sent Final ACK, Handshake complete.
```
## Code Description

### 1. **Packet Making**

The client is making raw TCP packets with use of raw sockets. Each packet has an IP header and a TCP header. The fields in those headers are populated manually to create the handshake.

- **The IP header**: Has information like source IP address, destination IP address, type of protocol, and length of packet.
- **The TCP header**: Fields like source and destination ports, sequence and acknowledgement numbers, and TCP flags (like: SYN, ACK).

### 2. **Send SYN**

The SYN packet is created by the send_syn function and sent to the server. This packet initiates the handshake with the TCP header flag set on SYN.

### 3. **Receive SYN-ACK**

The receive_syn_ack function waits for the SYN-ACK response from the server. The packet is verified with respect to TCP flags and acknowledgment number.

### 4. **Sending ACK**

The send_ack function creates an ACK packet for the completion of the handshake by acknowledging the server's SYN-ACK.

### 5. **Debugging Packet Sending/Receiving**

The print_tcp_packet function comprises a detailed printout of TCP packets actually being sent and received to/from the IP addresses, ports, sequence numbers, acknowledgment numbers, and flags.

## Team and Contributions
Team Members:
- Venkatesh Akula (220109)
- Mohammed Anas (220654)
- Sai Nikhil (221095)
  
Each contributed 33.3% to the assignment, including design, coding, testing, and documentation.

---

## Declaration
We declare that this code and its documentation are our own work. We have not used or provided any unauthorized material. All external references or influences have been properly acknowledged.
