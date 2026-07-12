add_library(ve_platform STATIC
    platform.cpp
    window.cpp
    input.cpp
)
find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(ve_platform PRIVATE ve_core SDL3::SDL3)