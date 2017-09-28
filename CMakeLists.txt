cmake_minimum_required(VERSION 3.8)
project(ISAbot)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp communicator.cpp communicator.h ircCommunicator.cpp ircCommunicator.h)
add_executable(ISAbot ${SOURCE_FILES})