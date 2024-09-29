cmake_minimum_required(VERSION 3.20)

project(extra-clicks VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# libevdev unimplemented
# option(ENABLE_EVDEV "Enable evdev emulated input driver backend" TRUE)
option(ENABLE_UINPUT "Enable uinput emulated input driver backend" TRUE)
option(ENABLE_EVENT "Enable use of event watcher backend" TRUE)

# if (ENABLE_EVDEV)
#     include(FindPkgConfig)
#     if (NOT PKG_CONFIG_FOUND)
#         message(FATAL_ERROR "PkgConfig was not found")
#     endif()

#     pkg_check_modules(LIBEVDEV REQUIRED libevdev)
# endif()

add_subdirectory(src)