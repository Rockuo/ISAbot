//
// Created by rockuo on 29.9.17.
//

#ifndef ISABOT_LOGGER_H
#define ISABOT_LOGGER_H

#include <sys/socket.h>
#include <iostream>
#include <netdb.h>
#include <vector>
#include <utility>


class Logger {
public:
    Logger();
    void start(std::string ip, std::vector<std::string> highlights);
    void log(std::string message);

private:
    int fd;
    std::string ip;
    std::vector<std::string> highlights;
    struct sockaddr_in sockaddrIn{};
};


#endif //ISABOT_LOGGER_H
