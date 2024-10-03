# Enables modules to inlcude files from other modules
include_directories(.)

add_compile_options(
    -Werror=all -Werror=extra -Werror=shadow
)

add_subdirectory(driver)
add_subdirectory(input)
add_subdirectory(turbo-clicker)