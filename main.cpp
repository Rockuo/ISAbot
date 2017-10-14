#include <iostream>
#include <csignal>
#include "Irc.h"
#include "Logger.h"

using namespace std;

void quit(int sig);
void help(vector<string> arguments);
string getSyslogServer(vector<string> arguments);
vector<string> getHighlights(vector<string> arguments);
void close();

vector<string> charArrayToVector(int length, char** array) {
    vector<string> vector;
    for (int i = 0; i< length; i++) {
        vector.emplace_back(string(array[i]));
    }
    return vector;
}


Irc irc;
Logger logger;

int main(int argc, char** argv) {
    vector<string> arguments = charArrayToVector(argc, argv);
    help(arguments);
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
    string syslogServer = getSyslogServer(arguments);
    if(syslogServer.empty()) {
        syslogServer = "127.0.0.1";
    }

    vector<string> highlights = getHighlights(arguments);
    logger.start(syslogServer, highlights);

    try {
        irc.start(host, port, channels, [](string message) -> void { logger.log(message); });
    } catch (IrcException& e) {
        cerr << e.what() << endl;
        close();
        return e.getCode();
    }
    return 0;
}

void quit(int sig) {
    close();
    exit(0);
}
void close() {
    irc.unlink();
    logger.unlink();
}

void help(vector<string> arguments) {
    for(const auto &arg : arguments) {
        if(arg == "-h" || arg == "--help") {
            cout << "help" << endl; //todo
            exit(0);
        }
    }
}
string getSyslogServer(vector<string> arguments) {
    bool match = false;
    for(auto arg : arguments) {
        if(arg == "-s") {
            match = true;
        } else if (match) {
            return arg;
        }
    }
    return string();
}
vector<string> getHighlights(vector<string> arguments) {
    vector<string> highlights;
    bool match = false;
    for(const auto &arg : arguments) {
        if(arg == "-l") {
            match = true;
        } else if (match) {
            size_t pos ;
            string copy = string(arg);
            while (true) {
                pos = copy.find(',');
                if(pos == std::string::npos) {
                    highlights.push_back(copy);
                    return highlights;
                }
                highlights.push_back(copy.substr(0, pos));
                copy.erase(0, pos + 1);
            }
        }
    }
    return highlights;
}
