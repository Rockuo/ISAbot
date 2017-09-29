//
// Created by rockuo on 29.9.17.
//

#include <arpa/inet.h>
#include <string.h>
#include "Logger.h"

#define self (*this)

Logger::Logger() = default;

void Logger::start(std::string ip, std::vector<std::string> highlights) {
    self.ip = ip;
    self.highlights = std::move(highlights);
    self.fd = socket(AF_INET, SOCK_DGRAM, 0);
    self.sockaddrIn.sin_family = AF_INET;
    self.sockaddrIn.sin_port = htons(514);
    self.sockaddrIn.sin_addr.s_addr = inet_addr(ip.c_str());
    memset(&(self.sockaddrIn.sin_zero), '\0', 8);
}

void Logger::log(std::string message) {
    // todo HIGhLIGHTS
    std::string log;
    size_t pos;
    if (message.find("PRIVMSG") != std::string::npos) {
        message.erase(0, 1);
        pos = message.find('!');
        std::string name = message.substr(0, pos);
        message.erase(0, pos + 1);
        pos = message.find(" :");
        message.erase(0, pos + 2);
        time_t now = time(nullptr);
        std::string timeString = ctime(&now);
        pos = timeString.find('\n');
        timeString = timeString.substr(0, pos);
        message = std::string("<134> ") + timeString + " " + self.ip + " isabot <" + name + ">:" + message; // todo check format
    } else if (message.find("NOTICE") != std::string::npos) {
        message.erase(0, 1);
        pos = message.find(" :");
        message.erase(0, pos + 2);
        time_t now = time(nullptr);
        std::string timeString = ctime(&now);
        pos = timeString.find('\n');
        timeString = timeString.substr(0, pos);
        message = std::string("<134> ") + timeString + " " + self.ip + " isabot " + message; // todo check format
    }


    sendto(self.fd, message.c_str(), message.length(), 0, (struct sockaddr *) &sockaddrIn, sizeof(self.sockaddrIn));
    std::cerr << "shoud log: " << message << std::endl;
}

