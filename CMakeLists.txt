cmake_minimum_required(VERSION 3.12)
project(Reservas)

find_package(mongocxx REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0079 NEW)

# Subdirectories
add_subdirectory(src)
add_subdirectory(lib)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    imgui
    d3d11
)