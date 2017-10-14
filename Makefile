SRC=main.cpp
CFLAGS=-std=c11
NAME=isabot

all: $(SRC)
	g++ -o $(NAME) $(SRC) Irc.cpp Logger.cpp


