set(CXX_FLAGS
    "-Wall"
    "-Wextra"
    "-Werror"
    "-std=c++14"
    "-pthread"
    "-pedantic"
    "-fno-rtti"
    "-fPIE")

foreach(FLAG ${CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}")
endforeach()

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -march=native ${LTO_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native ${LTO_FLAGS}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g ${LTO_FLAGS}")

set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined,-02 ${LTO_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${LTO_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${LTO_FLAGS}")

find_package(Threads REQUIRED)
find_package(Extra REQUIRED)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(DEBUG "true")
else()
    set(DEBUG "false")
endif()

configure_file("${PROJECT_SOURCE_DIR}/config.h.in"
               "${PROJECT_BINARY_DIR}/conf.h")
include_directories("${PROJECT_BINARY_DIR}")

set(CLIENT_EXECUTABLE "poppe" CACHE STRING "client executable name")
set(SERVER_EXECUTABLE "popper" CACHE STRING "server executable name")

add_executable(${SERVER_EXECUTABLE} mainsrv.cpp)
add_library(global global.cpp)
add_library(config config.cpp)
add_library(lockio lockio.cpp)
add_library(caller caller.cpp)
add_library(server server.cpp)
target_link_libraries(caller lockio)
target_link_libraries(server global)
target_link_libraries(server config)
target_link_libraries(server ${Extra_LIBRARIES})
target_link_libraries(server caller)
target_link_libraries(${SERVER_EXECUTABLE} server)
target_link_libraries(${SERVER_EXECUTABLE} ${CMAKE_THREAD_LIBS_INIT})

add_executable(${CLIENT_EXECUTABLE} maincli.cpp)
add_library(client client.cpp)
target_link_libraries(client global)
target_link_libraries(client ${Extra_LIBRARIES})
target_link_libraries(${CLIENT_EXECUTABLE} client)

add_custom_command(TARGET global PRE_BUILD
                   COMMAND python "${PROJECT_SOURCE_DIR}/scripts/passgen.py"
                   WORKING_DIRECTORY ${PROJECT_BINARY_DIR})

install(TARGETS ${SERVER_EXECUTABLE} DESTINATION bin)
install(TARGETS ${CLIENT_EXECUTABLE}  DESTINATION bin)
install(CODE "EXEC_PROGRAM(setcap ARGS \\\"CAP_SETPCAP=eip\\\"
        ${CMAKE_INSTALL_PREFIX}/bin/${SERVER_EXECUTABLE})")
