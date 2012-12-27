CXXFLAGS = -Wall -Iinclude
LDFLAGS = -Llib
LDLIBS = -lstdc++ -lm -lGL -lGLEW -lglfw

TARGET = main

$(TARGET) : main.o

main.o : main.cc
