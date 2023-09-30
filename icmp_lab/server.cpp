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

#define PORT 6000

int main(int argc, char* argv[]) {
    int listenfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen = sizeof(cliaddr);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    listen(listenfd, 5);

    while(true) {
        int connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);
        if(fork() == 0) {
            close(listenfd);
            char msg[512];
            int n = read(connfd, msg, 512);
            cout << "Client: " << msg << endl;
            write(connfd, msg, n);
            close(connfd);
            exit(0);
        }
        close(connfd);
    }

    return 0;
}
