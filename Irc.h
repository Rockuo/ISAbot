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
#include <cstring>
#include <vector>
#include <map>

using namespace std;

class IrcException: public exception
{
public:
    explicit IrcException(string message) {
        this->msg = move(message);
    };
    IrcException(string message, int code) {
        this->msg = move(message);
        this->code = code;
    };
    int getCode() { return code;};
    virtual const char* what() const throw()
    {
        return msg.c_str();
    }
private:
    int code = 1;
    string msg;
};


class UserList {
public:
    void add(const string &user) {  if (getIndex(user) == -1) { users.push_back(user); }};
    void remove(const string &user) {  int index = getIndex(user); if (index > -1) { users.erase(users.begin() + index); }};
    bool isFilled() { return filled; };
    void reset() {users.clear(); filled = false;};
    void setAsFilled() { filled = true; };
    int getIndex(const string &needle) {
        for (int i = 0; i < users.size(); i++) {
            if (users[i] == needle) {
                return i;
            }
        }
        return -1;
    }
    void renameUser(const string &from, string to) {
        for (auto &user : users) {
            if(user == from) {
                user = move(to);
                break;
            }
        }
    };
    vector<string> getUsers() {return users;};
private:
    bool filled;
    vector<string> users;
};

class Irc {
public:
    Irc();
    int unlink();
    void start(const std::string &host,  int port, vector<string> channels, const function<void(std::string)> &callback);
    ssize_t sendMessage(const std::string &string);

private:
    int fd;
    char buffer[513];
    string message; //redundant copy of buffer
    bool handlePing();
    bool joinMe(vector<string> channels);
    int receive();

    map<string, map<string, vector<string>>> chanelToUserToMessages;
    map<string, UserList> chanelUsers;
    bool handleUsers();
    void addUserToChannel(string user, string chanel);
    void removeUserFromChanel(std::string user, std::string chanel);
    void renameUser(std::string before, std::string after);
    bool catchJoin();
    bool catchLeave();
    void sendAllJoined();
    bool handleNICK();
    void handleErrors();
};


#endif //ISAMON_IRC_H
