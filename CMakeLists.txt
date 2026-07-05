cmake_minimum_required(VERSION 3.28)
project(voxel-world LANGUAGES CXX)

find_package(fmt CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
	string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	add_compile_options(/W4 /WX /permissive- /EHs-c- /GR-)
	add_compile_definitions(_HAS_EXCEPTIONS=0)
else()
	add_compile_options(-Wall -Wextra -Werror -fno-exceptions -fno-rtti)
endif()

add_subdirectory(engine/core)
add_subdirectory(engine/platform)
add_subdirectory(engine/rhi)
add_subdirectory(engine/renderer)
add_subdirectory(game)