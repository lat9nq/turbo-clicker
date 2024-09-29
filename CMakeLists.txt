cmake_minimum_required(VERSION 3.20)

project(extra-clicks VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

option(ENABLE_EVDEV "Enable use of evdev watcher backend" TRUE)

add_subdirectory(src)