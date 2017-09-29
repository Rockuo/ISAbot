#include <iostream>
#include <stdlib.h>
#include <signal.h>
#include "Irc.h"
#include "Logger.h"

using namespace std;
void quit(int sig);
string convertor(const string &message);
void help(char** argv);
string getSyslogServer(char** argv);
vector<string> getHighlights(char** argv);

Irc irc;
Logger logger;



int main(int argc, char** argv) {
    struct sigaction sa{};
    sa.sa_handler = &quit;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT,&sa, nullptr);

    string strChannels = argv[2];
    vector<string> channels;
    channels.clear();
    bool hasPort = string(argv[1]).find(':') != std::string::npos;
    string host = strtok (argv[1],":");
    int port = 6667;
    if(hasPort) {
        port = stoi(strtok(nullptr, ""));
    }
    size_t pos = 0;
    std::string token;
    while (true) {
        pos = strChannels.find(',');
        if(pos == std::string::npos) {
            break;
        }
        token = strChannels.substr(0, pos);
        channels.push_back(token);
        strChannels.erase(0, pos + 1);
    }
    channels.push_back(strChannels);
    //todo rest of params

    logger.start("aaa");

    irc.start(host, port, channels, [](string message) -> void{
        logger.log(convertor(message));
    });
    return 0;
}

string convertor(const string &message) {
    return "should be converted: " + message; //todo
}

void quit(int sig) {
    irc.sendMessage("QUIT\r\n");
    irc.unlink();
    exit(0);
}