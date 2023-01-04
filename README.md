# Pong

_Gilbert François Duivesteijn_



## Abstract

_Pong is a table tennis–themed twitch arcade sports video game, featuring simple two-dimensional graphics, manufactured by Atari and originally released in 1972._ (Source: Wikipedia)



## Install

```sh
mkdir -p build_external
git clone https://github.com/libsdl-org/SDL.git --branch release-2.26.1 --single-branch --depth 1 --recurse-submodules
cd SDL
./configure --enable-shared=no --enable-static=yes --prefix=`pwd`/../../ext/
```





## Motivation for making this game

- Improve skills in GDB / LLDB debugging

- Improve skills in multi-platform development

- Revive the 70s!

  

