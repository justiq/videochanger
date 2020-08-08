cmake_minimum_required (VERSION 3.1)

project(videochanger)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

set(SOURCE_EXE src/vidInfo.cpp src/imgInfo.cpp)

find_library(THREAD_LIB pthread)

include_directories(hdrs)

add_library(LIBS STATIC ${SOURCE_EXE})

add_executable(main src/videochanger.cpp)

target_link_libraries(main LIBS ${THREAD_LIB})

