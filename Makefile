FLAGS=-std=c++14 -g -Wall

INCLUDE_PATHS=-I/opt/homebrew/include
LIB_PATHS=-L/opt/homebrew/lib

all:
	c++ ${FLAGS} src/pong.cpp ${INCLUDE_PATHS} ${LIB_PATHS} -lSDL2 -lSDL2_ttf -o build/Pong	

clean:
	rm -Rf build/Pong
