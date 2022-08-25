set(
    third_party_INCLUDE_DIR
    "${CMAKE_CURRENT_LIST_DIR}/include"
    CACHE
    STRING
    "Include directory for various third party libraries included in this project."
    FORCE
)

add_library(
    stb STATIC
    "src/stb.c"
)

target_include_directories(
    stb PUBLIC INTERFACE
    "include/"
)

set_target_properties(
	stb PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${PROJECT_SOURCE_DIR}/debug/lib
	LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${PROJECT_SOURCE_DIR}/debug/lib
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/release/lib
	LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/release/lib
)