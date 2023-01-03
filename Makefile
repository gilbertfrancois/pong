CC_FLAGS=-O2 -Wall
CC_DEBUG_FLAGS=-g -Wall

INCLUDE_PATHS=-I/opt/homebrew/include
LIB_PATHS=-L/opt/homebrew/lib
STATIC_LIB_PREFIX=/opt/homebrew/lib
# INCLUDE_PATHS=-I/usr/local/include
# LIB_PATHS=-L/usr/local/lib

all:
	mkdir -p build/c
	cc ${CC_FLAGS} src/c/pong.c ${INCLUDE_PATHS} ${LIB_PATHS} -lSDL2 -lSDL2_ttf -o build/c/pong	

debug:
	mkdir -p build/c
	cc ${CC_DEBUG_FLAGS} src/c/pong.c ${INCLUDE_PATHS} ${LIB_PATHS} -lSDL2 -lSDL2_ttf -o build/c/pong	
	
static:
	mkdir -p build/c
	cc  -o build/c/pong	${CC_FLAGS} ${INCLUDE_PATHS} src/c/pong.c  ${STATIC_LIB_PREFIX}/libSDL2.a ${STATIC_LIB_PREFIX}/libSDL2main.a ${STATIC_LIB_PREFIX}/libSDL2_ttf.a
clean:
	rm -Rf build/c/
