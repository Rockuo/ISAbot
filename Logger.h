//
// Created by rockuo on 29.9.17.
//

#ifndef ISABOT_LOGGER_H
#define ISABOT_LOGGER_H

#include <sys/socket.h>
#include <iostream>
#include <netdb.h>
#include <cstring>
#include <arpa/inet.h>
#include <vector>
#include <utility>
#include <unistd.h>


class Logger {
public:
    Logger();
    void start(std::string ip, std::vector<std::string> highlights);
    void log(std::string message);
    int unlink();
private:
    int fd;
    std::string ip;
    std::vector<std::string> highlights;
    struct sockaddr_in sockaddrIn{};
    bool shouldLog(const std::string &message);
};


#endif //ISABOT_LOGGER_H
