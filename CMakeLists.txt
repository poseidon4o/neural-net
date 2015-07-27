cmake_minimum_required(VERSION 2.8)

project(nn)
ADD_DEFINITIONS(-std=c++11)

include_directories(net)
file(GLOB SOURCES "*.cpp" "problems/*.hpp")
file(GLOB HEADERS "net/*.hpp")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
