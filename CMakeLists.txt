cmake_minimum_required(VERSION 3.25)
project(pong)
set(CMAKE_CXX_STANDARD 14)

set(PROJECT_NAME pong)

# Find SDL2 and SDL2_ttf libraries
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES src/pong.cpp)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(pong src/pong.cpp)

# Link SDL2::Main, SDL2::Image and SDL2::GFX to our project
target_link_libraries(pong SDL2::SDL2 SDL2::SDL2main SDL2_ttf::SDL2_ttf)
