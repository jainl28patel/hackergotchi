#include<iostream>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#include<signal.h>
#include<strings.h>
#include<stdlib.h>

using namespace std;

int main(int argc, char* argv[]) {

    if(argc!=2) {
        printf("Usage: %s <ip_address>\n",argv[0]);
        exit(0);
    }

    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(6000);

    connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    cout << "Enter msg to send : ";
    char sendline[512], recvline[512];
    fgets(sendline, 512, stdin);
    int n = strlen(sendline);

    write(sockfd, sendline, n);
    n = read(sockfd, recvline, 512);
    if(n<0) {
        cout << "Error Reading" << endl;
    }

    recvline[n] = 0;
    fputs(recvline, stdout);

    return 0;
}