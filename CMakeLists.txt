cmake_minimum_required (VERSION 3.6)

project(cppbot)

add_executable(cppbot bot.cpp)

add_subdirectory(lib/sleepy-discord)

target_link_libraries(cppbot sleepy-discord)


