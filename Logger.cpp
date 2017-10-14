//
// Created by rockuo on 29.9.17.
//

#include <arpa/inet.h>
#include <cstring>
#include "Logger.h"

#define self (*this)
Logger::Logger() = default;

void Logger::start(std::string ip, std::vector<std::string> highlights) {
    self.highlights = std::move(highlights);
    self.fd = socket(AF_INET, SOCK_DGRAM, 0);
    self.sockaddrIn.sin_family = AF_INET;
    self.sockaddrIn.sin_port = htons(514);
    self.sockaddrIn.sin_addr.s_addr = inet_addr(ip.c_str());
    memset(&(self.sockaddrIn.sin_zero), '\0', 8);


    // write this to DOC
    struct sockaddr_in local;
    socklen_t len = sizeof(local);
    if (getsockname(self.fd, (struct sockaddr *) &local, &len) == -1){
        //todo fail
    }
    self.ip = inet_ntoa(local.sin_addr);


}

bool Logger::shouldLog(const std::string &message) {
    for(const std::string &h : self.highlights) {
        if(message.find(h + " ") == 0 ||
                message.find(" "+ h + " ") != std::string::npos ||
                message.find(" "+ h + "\n") != std::string::npos ||
                message.find("\n"+ h + " ") != std::string::npos ||
                message.find(" " + h + "\r") != std::string::npos ||
                message.find("\r" + h + "\n") != std::string::npos ||
                message == h+"\r\n") {
            return true;
        }
    }
    return false;
}


void Logger::log(std::string message) {
    std::string log;
    size_t pos;
    message.erase(0, 1);
    pos = message.find('!');
    std::string name = message.substr(0, pos);
    message.erase(0, pos + 1);
    if(name == message) {
        name = "";
    }
    pos = message.find(" :");
    message.erase(0, pos + 2);

    if (!shouldLog(message)) return;

    time_t t = time(nullptr);
    struct tm *now = localtime(&t);
    char TIMESTAMP[17];
    strftime(TIMESTAMP, 16, "%b %d %T", now); //Sep 21 16:00:00
    message = std::string("<134>") + TIMESTAMP + " " + self.ip + " isabot " + (!name.empty() ? name + ":": "") + message;

    sendto(self.fd, message.c_str(), message.length(), 0, (struct sockaddr *) &sockaddrIn, sizeof(self.sockaddrIn));
}

int Logger::unlink() {
    return close(self.fd);
}
