cmake_minimum_required(VERSION 2.8)

project(renzie)

include(Compiler.cmake)

set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")

add_subdirectory(src)

include(DebPack.cmake)
