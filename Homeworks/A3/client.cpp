#include <iostream>              
#include <cstring>               
#include <cstdlib>               
#include <sys/socket.h>       
#include <netinet/ip.h>          
#include <netinet/tcp.h>         
#include <arpa/inet.h>           
#include <unistd.h>              

#define SERVER_PORT 12345        // Server's listening port
#define CLIENT_PORT 54321        // Client's arbitrary high port

// Function to calculate checksum used for IP and TCP headers
// This is not a necessary computation as kernel anyways handles these packet feeds 
unsigned short checksum(unsigned short *ptr, int nbytes) {
    long sum;
    unsigned short oddbyte;
    unsigned short answer;

    sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }

    // If there's a remaining byte, pad and add
    if (nbytes == 1) {
        oddbyte = 0;
        *((unsigned char *)&oddbyte) = *(unsigned char *)ptr;
        sum += oddbyte;
    }

    // Add carry, wrap around
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = (short)~sum;  // Taking 1's complement

    return answer;
}

// Function to print IP and TCP header information
void print_tcp_packet(struct iphdr *ip, struct tcphdr *tcp, const char *label) {
    std::cout << "[" << label << "] Packet Info:\n";
    std::cout << "    Source IP: " << inet_ntoa(*(in_addr *)&ip->saddr) << "\n";
    std::cout << "    Dest IP:   " << inet_ntoa(*(in_addr *)&ip->daddr) << "\n";
    std::cout << "    Source Port: " << ntohs(tcp->source) << "\n";
    std::cout << "    Dest Port:   " << ntohs(tcp->dest) << "\n";
    std::cout << "    Seq: " << ntohl(tcp->seq)
              << " Ack: " << ntohl(tcp->ack_seq) << "\n";
    std::cout << "    Flags -> SYN: " << tcp->syn
              << " ACK: " << tcp->ack
              << " FIN: " << tcp->fin
              << " RST: " << tcp->rst
              << " PSH: " << tcp->psh << "\n";
}

// Function to construct and send SYN packet with sequence number 200.
// This is the first step of the three-way handshake from the client’s side.
void send_syn(int sock, struct sockaddr_in *server_addr) {
    char packet[sizeof(struct iphdr) + sizeof(struct tcphdr)];
    memset(packet, 0, sizeof(packet));

    struct iphdr *ip = (struct iphdr *)packet;
    struct tcphdr *tcp = (struct tcphdr *)(packet + sizeof(struct iphdr));

    // Fill IP header
    ip->ihl = 5;                           // Header length
    ip->version = 4;                       // To specify IPv4
    ip->tos = 0;                           
    ip->tot_len = htons(sizeof(packet));  // Total length
    ip->id = htons(12345);                // Identification
    ip->frag_off = 0;                      
    ip->ttl = 64;                          // Time to live
    ip->protocol = IPPROTO_TCP;           // Protocol (TCP)
    ip->saddr = inet_addr("127.0.0.1");   // Source IP
    ip->daddr = server_addr->sin_addr.s_addr; // Destination IP
    ip->check = checksum((unsigned short *)ip, sizeof(struct iphdr)); // IP checksum

    // Fill TCP header
    tcp->source = htons(CLIENT_PORT);     // Source port
    tcp->dest = htons(SERVER_PORT);       // Destination port
    tcp->seq = htonl(200);                // Sequence number
    tcp->ack_seq = 0;                     // Acknowledgment number
    tcp->doff = 5;                         // Data offset
    tcp->syn = 1;                          // SYN flag set
    tcp->window = htons(8192);            
    tcp->check = checksum((unsigned short *)tcp, sizeof(struct tcphdr)); 

    print_tcp_packet(ip, tcp, "SENT SYN");

    // Send SYN packet to server
    if (sendto(sock, packet, sizeof(packet), 0, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0) {
        perror("sendto() failed");
    }
}



// Function to send ACK in response to SYN-ACK
//This is the third and final step to complete the handshake.
// This ACK uses a sequence number of 600 and acknowledges the server's 400 by sending ack_seq=401.
void send_ack(int sock, struct sockaddr_in *server_addr, struct tcphdr *synack) {
    char packet[sizeof(struct iphdr) + sizeof(struct tcphdr)];
    memset(packet, 0, sizeof(packet));

    struct iphdr *ip = (struct iphdr *)packet;
    struct tcphdr *tcp = (struct tcphdr *)(packet + sizeof(struct iphdr));

    // Fill IP header
    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 0;
    ip->tot_len = htons(sizeof(packet));
    ip->id = htons(12346);
    ip->frag_off = 0;
    ip->ttl = 64;
    ip->protocol = IPPROTO_TCP;
    ip->saddr = inet_addr("127.0.0.1");
    ip->daddr = server_addr->sin_addr.s_addr;
    ip->check = checksum((unsigned short *)ip, sizeof(struct iphdr)); // IP checksum

    // Fill TCP header
    tcp->source = htons(CLIENT_PORT);
    tcp->dest = htons(SERVER_PORT);
    tcp->seq = htonl(600); // Next client sequence number
    tcp->ack_seq = htonl(ntohl(synack->seq) + 1); // Acknowledge server’s SYN
    tcp->doff = 5;
    tcp->ack = 1; // ACK flag set
    tcp->window = htons(8192);
    tcp->check = checksum((unsigned short *)tcp, sizeof(struct tcphdr)); // TCP checksum

    print_tcp_packet(ip, tcp, "SENT ACK");

    // Send ACK to complete handshake
    if (sendto(sock, packet, sizeof(packet), 0, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0) {
        perror("sendto() failed");
    }
}

// Function to receive SYN-ACK from server
// Waits for and verifies the SYN-ACK packet from the server.
// This is the second step in the three-way handshake where the server responds with both SYN and ACK flags set.
void receive_syn_ack(int sock) {
    char buffer[65536];                    // buffer for incoming packets
    struct sockaddr_in from_addr;
    socklen_t addr_len = sizeof(from_addr);

    while (true) {
        int data_size = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&from_addr, &addr_len);
        if (data_size < 0) {
            perror("recvfrom() failed");
            continue;
        }

        struct iphdr *ip = (struct iphdr *)buffer;
        struct tcphdr *tcp = (struct tcphdr *)(buffer + ip->ihl * 4);

        // Check if it's the SYN-ACK meant for us
        if (ntohs(tcp->dest) == CLIENT_PORT && tcp->syn == 1 && tcp->ack == 1 && ntohl(tcp->ack_seq) == 201) {
            print_tcp_packet(ip, tcp, "RECV SYN-ACK");

            // Step 3: If we get the expected SYN-ACK, send the final ACK.
            send_ack(sock, &from_addr, tcp);
            break;
        }
    }
}


int main() {
    // Create raw socket
    // This requires root user privileges
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int one = 1;
    // EEnable IP_HDRINCL to allow manual control over IP header fields in the raw socket.
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("setsockopt() failed");
        exit(EXIT_FAILURE);
    }

    // Prepare the server address structure.
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    std::cout << "[+] Sending SYN to server..." << std::endl;
    // Step 1: Send the SYN packet to initiate the handshake.
    send_syn(sock, &server_addr);      
    // Step 2: Wait for the SYN-ACK packet from the server.
    receive_syn_ack(sock);            
    printf("[+] Sent Final ACK, Handshake complete.\n");

    close(sock);                      // Close socket
    return 0;
}
