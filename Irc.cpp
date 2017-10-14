//
// Created by rockuo on 28.9.17.
//
#define self (*this)

#include "Irc.h"

#include <utility>

#define nickname "xbures29"
using namespace std;

Irc::Irc() = default;
// JOIN = IN ..to check
//PART =OUt kanal ..to check
//QUIT = out all ..to check
// KICK = out kanal ..to check
// NICK = zmena jmena ..to check
//todo 433 nickname in use ..to check
// todo 465 bann ..to check
// ERROR :Closing Link .. to check
int Irc::unlink() {
    self.sendMessage("QUIT\r\n");
    return close(self.fd);
}

int Irc::receive() {
    int index = -1;
    while (true) {
        index++;
        char item[2] = {'\0', '\0'};
        ssize_t numberRecv = recv(self.fd, item, 1, 0);
        if (numberRecv < 0) {
            throw IrcException("ERROR connection lost");
        } else {
            self.buffer[index] = item[0];
            if (item[0] == '\n' && self.buffer[index - 1] == '\r') {
                self.buffer[index + 1] = '\0';
                self.message = buffer;
                return index + 1;
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

bool Irc::joinMe(vector<std::string> channels) {
    if (self.message.find(string(" 001 ") + nickname + " :") != std::string::npos) {
        for (const auto &channel : channels) {
            sendMessage(string("JOIN ") + channel + "\r\n");
            self.chanelToUserToMessages[channel] = map<string, vector<string>>();
        }
        return true;
    }
    return false;
}


bool Irc::catchJoin() {
    if (self.message.find(" JOIN ") != std::string::npos) {
        self.message.erase(0, 1);
        size_t pos = self.message.find('!');
        string user = self.message.substr(0, pos);
        pos = self.message.find("JOIN ");
        self.message.erase(0, pos + 5);
        pos = self.message.find('\r');
        self.message.erase(pos);
        string channel = message;
        self.addUserToChannel(user, channel);
        if(self.chanelToUserToMessages[channel].find(user) != self.chanelToUserToMessages[channel].end()) {
            for(const string &userMessage: self.chanelToUserToMessages[channel][user]) {
                sendMessage(userMessage);
            }
            self.chanelToUserToMessages[channel][user].clear();
        }
        return true;
    }
    return false;
}

bool Irc::catchLeave() {
    bool isPart = self.message.find(" PART ") != std::string::npos;
    bool isQuit = self.message.find(" QUIT ") != std::string::npos;

    if (isPart || isQuit) {
        string user = string(self.message);
        user.erase(0, 1);
        size_t pos = user.find('!');
        user = user.substr(0, pos);

        if(isPart) {
            string channel = string(self.message);
            pos = channel.find(" #");
            channel.erase(0, pos + 2);
            self.removeUserFromChanel(user, channel);
        } else {
            for(pair<const string, UserList> & option: self.chanelUsers)
            {
                option.second.remove(user);
            }
        }
        return true;
    } else if (self.message.find(" KICK ") != std::string::npos) {
        string copy = string(self.message);
        size_t pos = copy.find("KICK ");
        copy.erase(0, pos + 5);
        pos = copy.find(' ');
        string chanel = copy.substr(0, pos);
        copy.erase(0, pos+1);
        pos = copy.find(' ');
        string user = copy.substr(0, pos);
        self.removeUserFromChanel(user, chanel);
        return true;
    }
    return false;
}

void Irc::sendAllJoined(){
    for(pair<const string, UserList> & chUPair: self.chanelUsers) {
        map<string, vector<string>> messageMap = self.chanelToUserToMessages[chUPair.first];
        for(string user: chUPair.second.getUsers()) {
            for(string userMessage: messageMap[user]) {
                sendMessage(userMessage);
            }
            self.chanelToUserToMessages[chUPair.first][user].clear();
        }
    }
}

bool Irc::handleUsers() {
    if (self.message.find(string("353 ") + nickname) != std::string::npos) {
        size_t pos = self.message.find("@ #");
        self.message.erase(0, pos + 2);
        pos = self.message.find(" :");
        string chanel = self.message.substr(0, pos);
        if(self.chanelUsers[chanel].isFilled()) {
            self.chanelUsers[chanel].reset();
        }
        self.message.erase(0, pos + 2);
        while (true) {
            pos = self.message.find(' ');
            if (pos == std::string::npos) {
                pos = self.message.find('\r');
                self.message.erase(pos);
                self.addUserToChannel(self.message, chanel);
                return true;
            }
            self.addUserToChannel(self.message.substr(0, pos), chanel);
            self.message.erase(0, pos + 1);
        }
    } else if (self.message.find(string("366 ") + nickname) != std::string::npos) {
        size_t pos = self.message.find(" #");
        self.message.erase(0, pos + 1);
        pos = self.message.find(" :");
        string chanel = self.message.substr(0, pos);
        self.chanelUsers[chanel].setAsFilled();
        self.sendAllJoined();
        return true;
    }
    return false;
}

void Irc::addUserToChannel(std::string user, std::string chanel) {
    if (self.chanelUsers.find(chanel) != self.chanelUsers.end()) {
        if(user.c_str()[0] == '@'){
            user.erase(0,1);
        }
        self.chanelUsers[chanel].add(user);
    }
}

void Irc::removeUserFromChanel(std::string user, std::string chanel) {
    if (self.chanelUsers.find(chanel) != self.chanelUsers.end()) {
        self.chanelUsers[chanel].remove(user);
    }
}

void Irc::renameUser(std::string before, std::string after) {
    for(pair<const string, UserList> & chanel: self.chanelUsers) {
        chanel.second.renameUser(before, after);
    }
}

bool Irc::handleNICK() {
    if(self.message.find(" NICK ") != std::string::npos) {
        string copy = string(self.message);
        size_t pos;
        copy.erase(0,1);
        pos = copy.find('!');
        string before = copy.substr(0, pos);
        pos = copy.find("NICK :");
        copy.erase(0, pos + 6);
        pos = copy.find('\r');
        string after = copy.substr(0, pos);
        self.renameUser(before, move(after));

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
    sockaddrIn.sin_addr = *((struct in_addr *) hostess_twinkies->h_addr);
    memset(&(sockaddrIn.sin_zero), '\0', 8);
    if (connect(self.fd, (struct sockaddr *) &sockaddrIn, sizeof(struct sockaddr)) < 0) {
        throw IrcException("ERROR Connectiong to IRC server");
    }
    sendMessage(string("USER ") + nickname + " 0 * :" + nickname + "\r\n");
    sendMessage(string("NICK ") + nickname + "\r\n");
    while (receive()) {
//        cerr << "......" << message;
        size_t pos;
        if (self.message.find("PRIVMSG") != std::string::npos || self.message.find("NOTICE") != std::string::npos) {
            if (self.message.c_str()[0] != ':') {
                continue;
            }
            if ((pos = self.message.find("?msg ")) != std::string::npos) {
                string messCont(self.message);
                string channel = messCont.substr(0, pos);
                messCont.erase(0, pos + 5);
                pos = self.message.find("PRIVMSG ");
                channel.erase(0, pos + 8);
                pos = channel.find(' ');
                channel = channel.substr(0, pos);
                pos = messCont.find(':');
                string name = messCont.substr(0, pos);

                if (self.chanelUsers[channel].getIndex(name) > -1) {
                    self.sendMessage("PRIVMSG " + channel + " :" + messCont);
                } else {
                    map<std::string, vector<std::string>> *users = &self.chanelToUserToMessages[channel];
                    if (users->find(name) == users->end()) {
                        vector<std::string> val = vector<std::string>();
                        (*users)[name] = val;
                    }
                    (*users)[name].push_back("PRIVMSG " + channel + " :" + messCont);
                }
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
                sendMessage(
                        "PRIVMSG " + channel + " :" + day + '.' + mon + '.' + to_string(now->tm_year + 1900) + "\r\n");
            }
            callback(self.message);
            continue;
        }
        if (self.handlePing()) continue;
        if (self.joinMe(channels)) continue;
        if (self.handleUsers()) continue;
        if (self.catchJoin()) continue;
        if (self.catchLeave()) continue;
        if (self.handleNICK()) continue;
        self.handleErrors();
    }
}

bool strHasSubstr(const string &msg, const string &needle) {
    return msg.find(needle) != std::string::npos;
}
void Irc::handleErrors() {
    if (self.message.find("ERROR :Closing Link") != std::string::npos) {
        throw IrcException("Connection Closed", 2);
    }
    else if(strHasSubstr(self.message, " 401 ")) { throw IrcException("No such nick/channel");}
    else if(strHasSubstr(self.message, " 402 ")) { throw IrcException("No such server");}
    else if(strHasSubstr(self.message, " 403 ")) { throw IrcException("No such channel");}
    else if(strHasSubstr(self.message, " 404 ")) { throw IrcException("Cannot send to channel");}
    else if(strHasSubstr(self.message, " 405 ")) { throw IrcException("You have joined too many");}
    else if(strHasSubstr(self.message, " 406 ")) { throw IrcException("There was no such nickname");}
    else if(strHasSubstr(self.message, " 407 ")) { throw IrcException("Duplicate recipients. No message");}
    else if(strHasSubstr(self.message, " 409 ")) { throw IrcException("No origin specified");}
    else if(strHasSubstr(self.message, " 411 ")) { throw IrcException("No recipient given");}
    else if(strHasSubstr(self.message, " 412 ")) { throw IrcException("No text to send");}
    else if(strHasSubstr(self.message, " 413 ")) { throw IrcException("No toplevel domain specified");}
    else if(strHasSubstr(self.message, " 414 ")) { throw IrcException("Wildcard in toplevel domain");}
    else if(strHasSubstr(self.message, " 422 ")) { throw IrcException("MOTD File is missing");}
    else if(strHasSubstr(self.message, " 431 ")) { throw IrcException("No nickname given");}
    else if(strHasSubstr(self.message, " 432 ")) { throw IrcException("Erroneus nickname");}
    else if(strHasSubstr(self.message, " 433 ")) { throw IrcException("Nickname is already in use");}
    else if(strHasSubstr(self.message, " 436 ")) { throw IrcException("Nickname collision KILL");}
    else if(strHasSubstr(self.message, " 441 ")) { throw IrcException("They aren't on that channel");}
    else if(strHasSubstr(self.message, " 442 ")) { throw IrcException("You're not on that channel");}
    else if(strHasSubstr(self.message, " 443 ")) { throw IrcException("user is already on channel");}
    else if(strHasSubstr(self.message, " 444 ")) { throw IrcException("User not logged in");}
    else if(strHasSubstr(self.message, " 445 ")) { throw IrcException("SUMMON has been disabled");}
    else if(strHasSubstr(self.message, " 446 ")) { throw IrcException("USERS has been disabled");}
    else if(strHasSubstr(self.message, " 451 ")) { throw IrcException("You have not registered");}
    else if(strHasSubstr(self.message, " 462 ")) { throw IrcException("You may not reregister");}
    else if(strHasSubstr(self.message, " 463 ")) { throw IrcException("Your host isn't among the privileged");}
    else if(strHasSubstr(self.message, " 464 ")) { throw IrcException("Password incorrect");}
    else if(strHasSubstr(self.message, " 465 ")) { throw IrcException("You are banned from this server");}
    else if(strHasSubstr(self.message, " 467 ")) { throw IrcException("Channel key already set");}
    else if(strHasSubstr(self.message, " 471 ")) { throw IrcException("Cannot join channel (+l)");}
    else if(strHasSubstr(self.message, " 473 ")) { throw IrcException("Cannot join channel (+i)");}
    else if(strHasSubstr(self.message, " 474 ")) { throw IrcException("Cannot join channel (+b)");}
    else if(strHasSubstr(self.message, " 475 ")) { throw IrcException("Cannot join channel (+k)");}
    else if(strHasSubstr(self.message, " 481 ")) { throw IrcException("Permission Denied- You're not an IRC operator");}
    else if(strHasSubstr(self.message, " 482 ")) { throw IrcException("You're not channel operator");}
    else if(strHasSubstr(self.message, " 483 ")) { throw IrcException("You cant kill a server!");}
    else if(strHasSubstr(self.message, " 491 ")) { throw IrcException("No O-lines for your host");}
};