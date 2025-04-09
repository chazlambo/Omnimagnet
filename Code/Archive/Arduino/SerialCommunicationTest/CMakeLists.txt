cmake_minimum_required(VERSION 3.10)
project(Arduino_Communication_Test)

find_package(Qt5 REQUIRED COMPONENTS Core SerialPort)


add_executable(ArduinoTest main.cpp)

