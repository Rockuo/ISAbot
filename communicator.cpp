//
// Created by rockuo on 28.9.17.
//

#include "communicator.h"

communicator::communicator() = default;

int communicator::link(const char* host, int port) {
    struct sockaddr_in si{};
    struct hostent *hostess_twinkies = gethostbyname(host);
    fd = socket(PF_INET, SOCK_STREAM, 0);
    si.sin_family = AF_INET;
    si.sin_port = htons(static_cast<uint16_t>(port));
    si.sin_addr = *((struct in_addr*)hostess_twinkies->h_addr);
    memset(&(si.sin_zero),'\0',8);
    if (connect(fd,(struct sockaddr*)&si,sizeof(struct sockaddr))==-1) fd=0;
    return fd;
}

int communicator::unlink() { return close(fd); }

int communicator::receive(char *buf) {
    int len;
    for (len=0;recv(fd,buf,1,0);len++,buf++) {
        if (*buf == '\n')
            if (*(buf - 1) == '\r') {
                *(buf + 1) = '\0';
                return len;
            }
    }

}
ssize_t communicator::sendChars(const char *buf) {
    int i;
    for (i=0;*(short*)buf!=0x0a0d;i++,buf++);
    buf-=i; i+=2;
    return send(fd, buf, static_cast<size_t>(i), 0);
}

ssize_t communicator::sendString(std::string string) {
    return sendChars(string.c_str());
}