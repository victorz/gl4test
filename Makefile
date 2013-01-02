CFLAGS = -Wall -std=c99 -Iinclude
LDFLAGS = -Llib
LDLIBS = -lm -lGL -lGLEW -lglfw

TARGET = main

$(TARGET) : main.o

.PHONY : clean

clean :
	rm -f *.o *~ core
