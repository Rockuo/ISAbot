<h2>Popis</h2>

ISAbot se připojuje k zadanému irc serveru a kanálu nad kterým poslouchá. Zašle-li někdo na server zprávu ?today - zašle zprávu na daný kanál s datem ve formátu dd.mm.yyyy, zašle-li někdo ?msg <nickname>:<msg> - pokud je uživatel na daném kanále, odešle mu zprávu okamžitě na tentýž kanál, pokud uživatel momentálně není na kanále, zprávu si uloží a odešle ji hned jakmile se daný uživatel připojí na daný kanál (obsah zprávy bude <nickname>:<msg>). Dále loguje přijaté zprávy na zadaný syslog server, pokud zpráva obsahuje jedno ze zadaných “HIGHLIGHT”.

<h2>Použití</h2>

isabot HOST[:PORT] CHANNELS [-s SYSLOG_SERVER] [-l HIGHLIGHT] [-h|--help] <br>
HOST je název serveru (např. irc.freenode.net) <br>
PORT je číslo portu, na kterém server naslouchá (výchozí 6667) <br>
CHANNELS obsahuje název jednoho či více kanálů, na které se klient připojí (název kanálu je zadán včetně úvodního # nebo &; v případě více kanálů jsou tyto odděleny čárkou)<br>
-s SYSLOG_SERVER je ip adresa logovacího (SYSLOG) serveru<br>
-l HIGHLIGHT seznam klíčových slov oddělených čárkou (např. "ip,tcp,udp,isa")<br>
<br>
<br>
<h5>Příklady použití:</h5>
`isabot irc.freenode.net:6667 "#ISAChannel,#IRC" -s 192.168.0.1 -l "ip,isa"`<br>
`isabot irc.freenode.net "#ISAChannel,#IRC" -l "ip,isa" -s 127.0.0.1`<br>
`isabot irc.freenode.net #ISAChannel -l "tcp,udp"`<br>

<h5>Příklad logu:</h5>
_<h6>Bot přijme IRC zprávu:</h6>_
`:xlogin00!xlogin00@asdf.cz PRIVMSG #ISAChannel :isa je nej`<br>
<h6>_a odešle SYSLOG zprávu:_</h6>
`<134>Sep 21 16:00:00 192.168.0.1 isabot <xlogin00>: isa je nej`

<h2>Soubory</h2>
Irc.cpp
Irc.h
Logger.cpp
Logger.h
main.cpp
Makefile
README.md