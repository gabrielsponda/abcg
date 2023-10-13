cmake_minimum_required(VERSION 3.11)

project(screensaver)

add_executable(${PROJECT_NAME} main.cpp window.cpp)

enable_abcg(${PROJECT_NAME})
