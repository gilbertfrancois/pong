# Retro Tennis

_Gilbert François Duivesteijn_

This project is work in progress. Please come back later...

## Abstract

_Retro Tennis, (also known as Pong) is a table tennis–themed twitch arcade sports video game, featuring simple two-dimensional graphics, manufactured by Atari and originally released in 1972._ (Source: Wikipedia)


## Install on macOS and Linux

Compiling the project should be straight forward, thanks to vcpkg. After compiling and installing, the program `pong` is located in the `<project_folder>/dist`.

```sh
git clone https://github.com/gilbertfrancois/retro-tennis.git

cd retro-tennis

# Important!
git submodule update --init --recursive

# Build the project and its dependencies
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
make install
```

## Running

```sh
cd dist
./pong
```
