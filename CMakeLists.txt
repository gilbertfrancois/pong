cmake_minimum_required(VERSION 3.18)

if(WASM)
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
find_package(SDL2_mixer CONFIG REQUIRED)

if(DEFINED EMSCRIPTEN)
  # Files to be preloaded and packed in the pong.data file.
  file(COPY_FILE ${CMAKE_SOURCE_DIR}/resources/sound/bounce.wav ${CMAKE_BINARY_DIR}/bounce.wav)
  file(COPY_FILE ${CMAKE_SOURCE_DIR}/resources/sound/pad.wav ${CMAKE_BINARY_DIR}/pad.wav)
  file(COPY_FILE ${CMAKE_SOURCE_DIR}/resources/sound/score.wav ${CMAKE_BINARY_DIR}/score.wav)
  set(CMAKE_EXECUTABLE_SUFFIX ".js")
  set(CMAKE_C_FLAGS "-s USE_SDL=2 -s ENVIRONMENT='web' -s DEFAULT_LIBRARY_FUNCS_TO_INCLUDE=$autoResumeAudioContext --preload-file ./bounce.wav --preload-file ./pad.wav --preload-file ./score.wav ")
endif()

add_executable(pong app/src/pong.c app/src/digit.c app/src/sound.c)
target_link_libraries(
  pong
  PRIVATE
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
)


if(DEFINED EMSCRIPTEN)
  install(TARGETS pong DESTINATION ${CMAKE_SOURCE_DIR}/web/src)
  install(FILES ${CMAKE_BINARY_DIR}/pong.wasm DESTINATION ${CMAKE_SOURCE_DIR}/web/src)
  install(FILES ${CMAKE_BINARY_DIR}/pong.data DESTINATION ${CMAKE_SOURCE_DIR}/web/src)
else()
  install(TARGETS pong DESTINATION ${CMAKE_SOURCE_DIR}/dist)
  install(FILES ${CMAKE_SOURCE_DIR}/resources/sound/bounce.wav DESTINATION ${CMAKE_SOURCE_DIR}/dist)
  install(FILES ${CMAKE_SOURCE_DIR}/resources/sound/pad.wav DESTINATION ${CMAKE_SOURCE_DIR}/dist)
  install(FILES ${CMAKE_SOURCE_DIR}/resources/sound/score.wav DESTINATION ${CMAKE_SOURCE_DIR}/dist)
endif()
