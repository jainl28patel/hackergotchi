#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;

int main() {
    int sockfd;
    struct ip *ip_hdr;
    struct icmphdr *icmp_hdr;
    struct sockaddr_in src_addr;
    socklen_t src_addr_len = sizeof(src_addr);
    char buffer[65536]; // Maximum packet size

    // Create a raw socket to capture ICMP packets
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        int packet_size = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&src_addr, &src_addr_len);

        if (packet_size == -1) {
            perror("Packet reception error");
            exit(EXIT_FAILURE);
        }

        ip_hdr = (struct ip *)buffer;
        icmp_hdr = (struct icmphdr *)(buffer + (ip_hdr->ip_hl << 2)); // Move to ICMP header

        // Check if it's an ICMP packet
        if (ip_hdr->ip_p == IPPROTO_ICMP) {
            cout << "Received ICMP packet" << endl;
            cout << "IP Header Length: " << (ip_hdr->ip_hl << 2) << " bytes" << endl;
        }
    }

    close(sockfd);

    return 0;
}

