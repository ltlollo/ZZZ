include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LAISENS")
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "ltlollo")
set(CPACK_PACKAGE_CONTACT "example@example.example")
set(CPACK_SET_DESTDIR on)
set(CPACK_DEBIAN_PACKAGE_DEPENDS
    "libsfml-system2 (>= 2.1+dfsg2-1),
     libsfml-audio2 (>= 2.1+dfsg2-1)")

include(CPack)