add_library(input
    input.cpp
    input.h
    buttons.h)

if (ENABLE_EVENT)
    target_sources(input PUBLIC
        event.cpp
        event.h)
endif()

target_link_libraries(input PRIVATE driver)