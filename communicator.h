//
// Created by rockuo on 28.9.17.
//

#ifndef ISAMON_COMMUNICATOR_H
#define ISAMON_COMMUNICATOR_H

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <string.h>

class communicator {
public:
    int link(const char* host, int port);
    int unlink();
    int receive(char *buf);
    ssize_t sendChars(const char *buf);
    ssize_t sendString(std::string string);
    communicator();
private:
    int fd;
};

#endif //ISAMON_COMMUNICATOR_H
