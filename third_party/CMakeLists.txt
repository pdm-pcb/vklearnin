set(
    third_party_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include"
    CACHE STRING
    "Include directory for various third party libraries included in this project."
    FORCE
)

add_library(
    third_party STATIC
    "src/stb.c"
)

target_include_directories(
    third_party PUBLIC
    "include/"
)

target_compile_options(
    third_party PRIVATE
    "-w"
)

set_target_properties(
	third_party PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${PROJECT_SOURCE_DIR}/debug/lib
	LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${PROJECT_SOURCE_DIR}/debug/lib
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/release/lib
	LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/release/lib
)

get_target_property(third_party_flags third_party COMPILE_OPTIONS)
get_target_property(third_party_defs  third_party COMPILE_DEFINITIONS)
get_target_property(third_party_inc   third_party INCLUDE_DIRECTORIES)
get_target_property(third_party_link  third_party LINK_DIRECTORIES)

message(STATUS "third_party compile flags: ${third_party_flags}")
message(STATUS "third_party compile defines: ${third_party_defs}")
message(STATUS "third_party include directories: ${third_party_inc}")
message(STATUS "third_party link directories: ${third_party_link}")
message(STATUS "cmake compile flags: ${CMAKE_CXX_FLAGS}")