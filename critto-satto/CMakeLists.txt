cmake_minimum_required(VERSION 2.8)

project(critto-satto)

include(Compiler.cmake)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "1")
set(VERSION_PATCH "1")

include_directories(${PROJECT_SOURCE_DIR}/src)
add_subdirectory(src)

include(DebPack.cmake)
