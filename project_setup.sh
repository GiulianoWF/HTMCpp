#!/bin/bash

rm -rf build
mkdir build
conan install . --output-folder=build --build=missing -s build_type=Debug
cd build
cmake .. -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=g++
cmake --build .
