add_library(ve_core STATIC
	log.cpp
)
target_include_directories(ve_core PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(ve_core PUBLIC fmt::fmt)