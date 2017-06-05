cmake_minimum_required(VERSION 2.8)

project(find-similar)

include(Compiler.cmake)

add_subdirectory(src)

set(VERSION_MAJOR "3")
set(VERSION_MINOR "2")
set(VERSION_PATCH "0")

include(DebPack.cmake)
