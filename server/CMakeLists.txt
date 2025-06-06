cmake_minimum_required(VERSION 3.10)

project(ZappyServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include header files
include_directories(include)

# Add the executable target
file(GLOB SOURCES "src/*.c")
file(GLOB SOURCES_FUNCTIONAL "src/functional/*.c")
add_executable(zappy_server ${SOURCES} ${SOURCES_FUNCTIONAL})