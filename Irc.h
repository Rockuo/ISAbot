//
// Created by rockuo on 28.9.17.
//

#ifndef ISAMON_IRC_H
#define ISAMON_IRC_H

#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <netdb.h>
#include <functional>
#include <string.h>
#include <vector>
#include <map>

using namespace std;



class Irc {
public:
    Irc();
    int unlink();
    int receive();
    void start(const std::string &host,  int port, vector<string> channels, const function<void(std::string)> &callback);
    ssize_t sendMessage(const std::string &string);

private:
    int fd;
    char buffer[513];
    string message; //redundant copy of buffer
    map<string, vector<string>> userToMessages;
    vector<string> users;
    bool handlePing();
    bool handleMyJoin(vector<string> channels);
    bool handleNames();
    bool handleElseJoin();
};

#endif //ISAMON_IRC_H
