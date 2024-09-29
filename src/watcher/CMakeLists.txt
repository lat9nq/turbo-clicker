add_library(watcher
    watcher.cpp
    watcher.h)

if (ENABLE_EVDEV)
    target_sources(watcher PUBLIC
        event.cpp
        event.h)
endif()