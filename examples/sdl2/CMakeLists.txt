cmake_minimum_required(VERSION 3.18.0)

add_executable(stream_SDL2 main.cpp)

find_package(SDL2 CONFIG REQUIRED)

target_link_libraries(stream_SDL2 PRIVATE
    stream
    SDL2::SDL2
    SDL2::SDL2main
)

set_target_properties(stream_SDL2 PROPERTIES CXX_STANDARD 20)
