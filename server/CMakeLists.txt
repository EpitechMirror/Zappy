cmake_minimum_required(VERSION 3.10)

project(ZappyServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include header files
include_directories(include)

# Add the executable target
add_executable(zappy_server
    src/main.c
    src/parse.c
)