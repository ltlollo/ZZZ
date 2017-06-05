if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set(LTO_FLAGS "-flto -fno-use-linker-plugin -fwhole-program")
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -pthread -std=c++11 -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -march=native ${LTO_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DNDEBUG -march=native ${LTO_FLAGS}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g ${LTO_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined -O2")
set(CMAKE_EXE_LINKER_FLAGS "${LTO_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${LTO_FLAGS}")

set(EXECUTABLE_NAME "${PROJECT_NAME}")
add_executable(${EXECUTABLE_NAME} main.cpp)

set(CMAKE_MODULE_PATH "/usr/share/SFML/cmake/Modules/")
set(RES_PATH "${PROJECT_SOURCE_DIR}/res")

configure_file("${PROJECT_SOURCE_DIR}/config.h.in"
               "${PROJECT_BINARY_DIR}/config.h")

include_directories("${PROJECT_BINARY_DIR}")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED system audio)
if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
endif()

install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
install(DIRECTORY ${RES_PATH} DESTINATION share/${PROJECT_NAME})
