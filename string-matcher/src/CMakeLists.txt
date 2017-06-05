set(CXX_FLAGS
    "-Wall"
    "-Wextra"
    "-Werror"
    "-pthread"
    "-std=c++14"
    "-pedantic"
    "-fno-rtti"
    "-fPIE")

foreach(FLAG ${CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}")
endforeach()

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set(LTO_FLAGS "-flto -fwhole-program")
endif()

set(OPT_FLAGS "-ffast-math -funroll-all-loops -march=native -Ofast")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -march=native ${LTO_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG ${OPT_FLAGS} ${LTO_FLAGS}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g ${LTO_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined -O2")
set(CMAKE_EXE_LINKER_FLAGS "${LTO_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${LTO_FLAGS}")

set(EXECUTABLE_NAME "${PROJECT_NAME}")

find_package(Work REQUIRED)
include_directories(${Work_INCLUDE_DIR})

aux_source_directory(. SRC_LIST)
add_executable(${EXECUTABLE_NAME} ${SRC_LIST})

install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
