set(CXX_FLAGS
    "-Wall"
    "-Wextra"
    "-Werror"
    "-pthread"
    "-std=c++1y"
    "-pedantic"
    "-fno-rtti"
    "-fPIE")

foreach(FLAG ${CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}")
endforeach()

set(PROJECT_INSTALL_DIR "work")

configure_file("${PROJECT_SOURCE_DIR}/INSTALL/libwork.pc.in"
               "${PROJECT_BINARY_DIR}/libwork.pc")

file(GLOB headers RELATIVE ${CMAKE_SOURCE_DIR}/src "*.h")
add_library(work SHARED ${headers} work.cpp)

install(DIRECTORY . DESTINATION include/${PROJECT_INSTALL_DIR}
        FILES_MATCHING PATTERN "*.h")
install(FILES "${PROJECT_BINARY_DIR}/libwork.pc"
        DESTINATION lib/pkgconfig)
install(FILES "${PROJECT_SOURCE_DIR}/INSTALL//FindWork.cmake"
        DESTINATION "/usr/share/cmake-3.0/Modules")
