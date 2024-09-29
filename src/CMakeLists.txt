# Enables modules to inlcude files from other modules
include_directories(.)

add_compile_options(
    -Werror=all -Werror=extra
)

add_subdirectory(driver)
add_subdirectory(watcher)
add_subdirectory(extra-clicker)