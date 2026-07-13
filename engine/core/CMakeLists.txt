add_library(ve_core STATIC
	log.cpp
	time.cpp
)

target_compile_definitions(ve_core PUBLIC $<$<CONFIG:Debug>:VE_ASSERTS_ENABLED=1>)

find_package(fmt CONFIG REQUIRED)
target_include_directories(ve_core PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(ve_core PUBLIC fmt::fmt)