find_package(PkgConfig)
pkg_check_modules(PC_LIBWORK libwork)
set(LIBWORK_DEFINITIONS ${PC_LIBWORK_CFLAGS_OTHER})

find_path(Work_INCLUDE_DIR workers.h
          HINTS ${PC_LIBWORK_INCLUDEDIR} ${PC_LIBWORK_INCLUDE_DIRS})

set(Work_INCLUDE_DIRS ${Work_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibWork DEFAULT_MSG Work_INCLUDE_DIR)

mark_as_advanced(Work_INCLUDE_DIR)
