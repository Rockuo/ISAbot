#include <iostream>
#include <stdlib.h>
#include <signal.h>
#include "communicator.h"
using namespace std;
void help(char *fname);
void quit(int sig);

communicator s = communicator();

int main(int argc, char** argv) {
    if (argc<5) help(argv[0]);

    /* Map CTRL-C to quit() */
    struct sigaction sa{};
    sa.sa_handler = &quit;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT,&sa,0);
    string channel = argv[3], nickname = argv[4];
    char buf[513];

    /* Connect to host (arg1) on port (arg2) */
    s.link(argv[1],atoi(argv[2]));

    /* The rest should be self-explanatory */
    s.sendString(string("USER ") + argv[4] + " 0 * :" + argv[4] + "\r\n");
    s.sendString(string("NICK ") + argv[4] + "\r\n");
    string message;
    while (s.receive(buf)) {
        message = buf;
        if (strstr(buf,"PING :") == buf) {
            buf[1] = 'O';
            s.sendChars(buf);
        } else if (message.find(" 001 "+ nickname +" :") != std::string::npos) {
            s.sendString(string("JOIN ") + argv[3] + "\r\n");
        } else {
            cout << buf;
        }
    }
    s.unlink();
    return 0;
}

void help(char *fname) {
    cout << "Usage: " << fname << " hostname port channel nick" << endl;
    exit(1);
}

void quit(int sig) {
    s.sendString("QUIT :Someone pressed CTRL-C on me.\r\n");
    s.unlink();
    exit(0);
}