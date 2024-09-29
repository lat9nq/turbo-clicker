add_library(watcher
    watcher.cpp
    watcher.h)

if (ENABLE_EVENT)
    target_sources(watcher PUBLIC
        event.cpp
        event.h)
endif()

target_link_libraries(watcher PRIVATE driver)