cmake_minimum_required(VERSION 3.0)

project(libwork-dev)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "6")
set(VERSION_PATCH "1")

add_subdirectory(src)

include(DebPack.cmake)
