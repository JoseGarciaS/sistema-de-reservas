cmake_minimum_required(VERSION 3.12)
project(Reservas)

find_package(mongocxx REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0079 NEW)

add_executable(${PROJECT_NAME} src/main.cpp)

# Subdirectories
add_subdirectory(src)
add_subdirectory(imgui)

