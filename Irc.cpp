//
// Created by rockuo on 28.9.17.
//
#define self (*this)
#include "Irc.h"
#define nickname "testbot12"
using namespace std;
Irc::Irc() = default;

int Irc::unlink() { return close(fd); }

int Irc::receive() {
    int index = -1;
    while(true) {
        index++;
        char item[2] = {'\0', '\0'};
        ssize_t numberRecv = recv(self.fd, item, 1,0);
        if (numberRecv < 0) {
            std::cerr <<"ERROR connection lost" <<std::endl;
            throw 1;
        } else {
            self.buffer[index] = item[0];
            if(item[0] == '\n') {
                self.buffer[index+1] = '\0';
                self.message = buffer;
                return index+1;
            }
        }

    }
}

ssize_t Irc::sendMessage(const std::string &string) {
    return send(self.fd, string.c_str(), string.length(), 0);
}

bool Irc::handlePing() {
    if (self.message.find("PING :") != std::string::npos) {
        self.buffer[1] = 'O';
        sendMessage(self.buffer);
        return true;
    }
    return false;
}

bool Irc::handleMyJoin(vector<std::string> channels){
    if (self.message.find(string(" 001 ") + nickname +" :") != std::string::npos) {
        for (const auto &channel : channels) {
            sendMessage(string("JOIN ") + channel + "\r\n");
        }
        return true;
    }
    return false;
}

bool Irc::handleNames() {
    if(self.message.find(string("353 ") + nickname) != std::string::npos) {
        size_t pos = self.message.find(" :");
        self.message.erase(0, pos + 2);
        while (true) {
            pos = self.message.find(' ');
            if(pos == std::string::npos) {
                return true;
            }
            self.users.push_back(self.message.substr(0, pos));
            self.message.erase(0, pos + 1);
        }
    } else if (self.message.find(string("366 ") + nickname) != std::string::npos) {
        for(const auto &user: self.users) {
            if (self.userToMessages.find(user) != self.userToMessages.end()) {
                for (const auto &messageToSend: self.userToMessages[user]) {
                    self.sendMessage(messageToSend);
                }
                self.userToMessages[user].clear();
            }
        }
        self.users.clear();
        return true;
    }
    return false;
}

bool Irc::handleElseJoin() {
    if (self.message.find(" JOIN ") != std::string::npos) {
        self.message.erase(0,1);
        size_t pos = self.message.find('!');
        string user = self.message.substr(0, pos);
        if (self.userToMessages.find(user) != self.userToMessages.end()) {
            for (const auto &messageToSend: self.userToMessages[user]) {
                self.sendMessage(messageToSend);
            }
            self.userToMessages[user].clear();
        }
        return true;
    }
    return false;
}

void Irc::start(
        const std::string &host,
        int port,
        vector<std::string> channels,
        const function<void(std::string)> &callback
) {
    struct sockaddr_in sockaddrIn{};
    struct hostent *hostess_twinkies = gethostbyname(host.c_str());
    self.fd = socket(PF_INET, SOCK_STREAM, 0); //SOCK_DGRAM
    sockaddrIn.sin_family = AF_INET;
    sockaddrIn.sin_port = htons(static_cast<uint16_t>(port));
    sockaddrIn.sin_addr = *((struct in_addr*)hostess_twinkies->h_addr);
    memset(&(sockaddrIn.sin_zero),'\0',8);
    if (connect(self.fd, (struct sockaddr*)&sockaddrIn, sizeof(struct sockaddr)) < 0) {
        std::cerr <<"ERROR connecting" <<std::endl;
        throw 1;
    }
    sendMessage(string("USER ") + nickname + " 0 * :" + nickname + "\r\n");
    sendMessage(string("NICK ") + nickname + "\r\n");
    while (receive()) {
        cerr << "......" << message;
        if(self.handlePing()) continue;
        if(self.handleMyJoin(channels)) continue;
        if(self.handleNames()) continue;
        if(self.handleElseJoin()) continue;

        size_t pos;
        if (self.message.find("PRIVMSG") != std::string::npos || self.message.find("NOTICE") != std::string::npos) {
            if ((pos = self.message.find("?msg ")) != std::string::npos) {
                string messCont(self.message);
                string chanel = messCont.substr(0, pos);
                messCont.erase(0, pos + 5);
                pos = self.message.find("PRIVMSG ");
                chanel.erase(0, pos + 8);
                pos = chanel.find(' ');
                chanel = chanel.substr(0, pos);
                pos = messCont.find(':');
                string name = messCont.substr(0, pos);
                messCont.erase(0, pos + 1);
                sendMessage("NAMES " + chanel + "\r\n");
                if (self.userToMessages.find(name) == self.userToMessages.end()) {
                    vector<string> val = vector<string>();
                    self.userToMessages[name] = val;
                }
                self.userToMessages[name].push_back("PRIVMSG " + name + " :" + messCont);
            } else if (self.message.find("?today") != std::string::npos) {
                pos = self.message.find("PRIVMSG");
                string copy(self.message);
                copy.erase(0, pos + 8);
                pos = copy.find(" :?today");
                string channel = copy.substr(0, pos);
                time_t t = time(nullptr);
                struct tm *now = localtime(&t);
                int iday = now->tm_mday, imon = now->tm_mon + 1;
                string day = iday < 10 ? "0" + to_string(iday) : to_string(iday);
                string mon = imon < 10 ? "0" + to_string(imon) : to_string(imon);
                sendMessage("PRIVMSG " + channel + " :" + day + '.' + mon + '.' + to_string(now->tm_year + 1900) + "\r\n");
            }
            callback(self.message);
        }
    }
}



