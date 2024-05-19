cmake_minimum_required(VERSION 3.18)

project(raycasting LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

add_executable(main main.cpp map.cpp player.cpp renderer.cpp)

target_link_libraries(main sfml-graphics sfml-window sfml-system)