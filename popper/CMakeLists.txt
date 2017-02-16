cmake_minimum_required(VERSION 2.8)

project(popper)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "8")
set(VERSION_PATCH "2")

add_subdirectory(src)

include(DebPack.cmake)
