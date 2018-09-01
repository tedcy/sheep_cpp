#include "client.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>//inet_addr
#include <netinet/tcp.h>//nodelay
#include <iostream>
#include <string.h>
using namespace std;

int Client::Echo() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    short port = 12345;
    struct sockaddr_in servaddr;  
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr((char*)"127.0.0.1");  
    servaddr.sin_port = htons(port); 
    if (connect(sockfd, (sockaddr *)&servaddr, sizeof(sockaddr_in)) < 0) {
        return -1;
    }
    char *s = (char*)"123456";
    ssize_t n = write(sockfd, s, 6);
    if (n < 0) {
        return -1;
    }
    s = new char[100];
    n = read(sockfd, s, 11);
    if (n < 0) {
        return -1;
    }
    close(sockfd);
    return 0;
}
