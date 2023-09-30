#include<iostream>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<cstring>

int main(){
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(sock < 0){
        std::cout << "Error in creating socket" << std::endl;
        return -1;
    }
    // set the destination address
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(0);
    dest.sin_addr.s_addr = inet_addr("127.0.0.1");
    // // set the source address
    // struct sockaddr_in src;
    // src.sin_family = AF_INET;
    // src.sin_port = htons(0);
    // src.sin_addr.s_addr = inet_addr("192.168.1.1");
    // bind the socket
    // if(bind(sock, (struct sockaddr*)&src, sizeof(src)) < 0){
    //     std::cout << "Error in binding socket" << std::endl;
    //     return -1;
    // }
    // send the packet
    char buffer[4096];
    memset(buffer, 0, 4096);
    if(sendto(sock, buffer, 4096, 0, (struct sockaddr*)&dest, sizeof(dest)) < 0){
        std::cout << "Error in sending packet" << std::endl;
        return -1;
    }
    return 0;
}