cmake_minimum_required(VERSION 3.18)

if(WITH_WASM)
  set(VCPKG_TARGET_TRIPLET "wasm32-emscripten")
  set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE
      "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"
      CACHE STRING "emscripten toolchain file")
endif()
set(CMAKE_TOOLCHAIN_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

project(pong)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)

if(DEFINED EMSCRIPTEN)
  add_executable(pong src/c/pong.c)
  set(CMAKE_EXECUTABLE_SUFFIX ".js")
  set_target_properties(pong PROPERTIES COMPILE_FLAGS " -s USE_SDL=2 ")
  # set_target_properties(main PROPERTIES LINK_FLAGS "-Os -s WASM=1 -s SIDE_MODULE=1 -s STANDALONE_WASM --no-entry")
else()
  add_executable(pong src/c/pong.c)
endif()

target_link_libraries(
  pong
  PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
    $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
)

install(TARGETS pong DESTINATION ${CMAKE_SOURCE_DIR}/dist)
file(COPY ${CMAKE_SOURCE_DIR}/resources/sound/bounce.wav DESTINATION ${CMAKE_SOURCE_DIR}/dist)
file(COPY ${CMAKE_SOURCE_DIR}/resources/sound/pad.wav DESTINATION ${CMAKE_SOURCE_DIR}/dist)
file(COPY ${CMAKE_SOURCE_DIR}/resources/sound/score.wav DESTINATION ${CMAKE_SOURCE_DIR}/dist)
if(DEFINED EMSCRIPTEN)
  install(FILES ${CMAKE_BINARY_DIR}/pong.wasm DESTINATION ${CMAKE_SOURCE_DIR}/dist)
  install(FILES ${CMAKE_SOURCE_DIR}/src/html/index.html DESTINATION ${CMAKE_SOURCE_DIR}/dist)
endif()

