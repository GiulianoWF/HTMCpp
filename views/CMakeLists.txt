cmake_minimum_required(VERSION 3.22)
project(Views)

set(CMAKE_CXX_STANDARD 20)

add_library(Views SHARED)

#For each directory with name <name>, add to the Views target the file <name>/<name>.cc
file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
set(subdir_sources "")
foreach(child ${children})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child})
        list(APPEND subdir_sources ${child}/view_${child}.cc)
        target_include_directories(Views PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${child})
    endif()
endforeach()

target_sources(Views PRIVATE ${subdir_sources})

target_link_libraries(Views PUBLIC Database fmt::fmt)
