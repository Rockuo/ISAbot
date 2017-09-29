//
// Created by rockuo on 29.9.17.
//

#ifndef ISABOT_LOGGER_H
#define ISABOT_LOGGER_H

#include <sys/socket.h>
#include <iostream>
#include <netdb.h>



class Logger {
public:
    Logger();
    void start(std::string ip);
    void log(const std::string &message);

private:
    int fd;
};


#endif //ISABOT_LOGGER_H
