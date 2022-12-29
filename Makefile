FLAGS=-std=c++11 -g -Wall

# INCLUDE_PATHS=-I/opt/homebrew/include
# LIB_PATHS=-L/opt/homebrew/lib
INCLUDE_PATHS=-I/usr/local/include
LIB_PATHS=-L/usr/local/lib

all:
	c++ ${FLAGS} src/pong.cpp ${INCLUDE_PATHS} ${LIB_PATHS} -lSDL2 -lSDL2_ttf -o build/pong	

clean:
	rm -Rf build/*
