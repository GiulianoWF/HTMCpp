cmake_minimum_required(VERSION 3.22)
project(dafult_project)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost REQUIRED)

add_subdirectory(http)

add_executable(executable main.cpp)
target_link_libraries(executable boost::boost Server ) 

