#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>  // Added for DNS lookup

// ICMP packet structure
struct icmphdr {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t id;
    uint16_t seq;
};

// Calculate ICMP checksum
uint16_t checksum(void *data, int len) {
    uint16_t *buf = static_cast<uint16_t *>(data);
    uint32_t sum = 0;
    while (len > 1) {
        sum += *buf++;
        len -= 2;
    }
    if (len == 1) {
        sum += static_cast<uint16_t>(*reinterpret_cast<uint8_t *>(buf));
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    return static_cast<uint16_t>(~(sum + (sum >> 16)));
}

// Perform DNS lookup for a domain name
char* dns_lookup(const char *hostname) {
    struct hostent *host_entity;
    struct in_addr **addr_list;

    if ((host_entity = gethostbyname(hostname)) == nullptr) {
        return nullptr;
    }

    addr_list = reinterpret_cast<struct in_addr **>(host_entity->h_addr_list);

    if (addr_list[0] == nullptr) {
        return nullptr;
    }

    return inet_ntoa(*addr_list[0]);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <destination domain name>" << std::endl;
        return 1;
    }

    const char *dest_domain = argv[1];
    char *dest_ip = dns_lookup(dest_domain);

    if (dest_ip == nullptr) {
        std::cerr << "DNS lookup failed! Could not resolve hostname: " << dest_domain << std::endl;
        return 1;
    }

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    inet_pton(AF_INET, dest_ip, &(dest_addr.sin_addr));

    // ICMP packet structure
    struct icmphdr icmp_hdr;
    icmp_hdr.type = ICMP_ECHO;
    icmp_hdr.code = 0;
    icmp_hdr.id = htons(getpid() & 0xFFFF);
    icmp_hdr.seq = htons(1);
    icmp_hdr.checksum = 0;
    icmp_hdr.checksum = checksum(&icmp_hdr, sizeof(icmp_hdr));

    // Ping loop
    while (true) {
        // Send ICMP packet
        int bytes_sent = sendto(sockfd, &icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (bytes_sent == -1) {
            perror("sendto");
            break;
        }

        // Receive ICMP reply
        unsigned char buffer[4096];
        struct sockaddr_in recv_addr;
        socklen_t addr_len = sizeof(recv_addr);

        int bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&recv_addr, &addr_len);
        if (bytes_received == -1) {
            perror("recvfrom");
            break;
        }

        // Process the reply (you can add more checks and print additional information)
        std::cout << "Received " << bytes_received << " bytes from " << inet_ntoa(recv_addr.sin_addr) << std::endl;

        sleep(1);  // Adjust the delay between pings (1 second in this example)
    }

    close(sockfd);

    return 0;
}

// /Users/jainilpatel/hackergotchi/icmp_lab/ping.cpp