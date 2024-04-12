cmake_minimum_required(VERSION 3.0.0)
project(HurnoConverter VERSION 0.1.0 LANGUAGES C CXX)

add_executable(${PROJECT_NAME} Converter.cpp)

find_library(Hurno Hurno)
target_link_libraries(${PROJECT_NAME} Hurno)