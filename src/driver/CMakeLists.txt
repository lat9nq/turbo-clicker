add_library(driver
    driver.cpp
    driver.h
    dummy.cpp
    dummy.h)

if (ENABLE_UINPUT)
    target_sources(driver PUBLIC
        uinput.cpp
        uinput.h)
endif()

# if (ENABLE_EVDEV)
#     target_sources(driver PUBLIC
#         evdev.cpp
#         evdev.h)

#     target_link_libraries(driver PRIVATE ${LIBEVDEV_LIBRARIES})
#     target_include_directories(driver PRIVATE ${LIBEVDEV_INCLUDE_DIRS})
# endif()