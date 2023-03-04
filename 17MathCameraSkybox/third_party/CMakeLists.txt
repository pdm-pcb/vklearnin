file(
    GLOB_RECURSE LIBRARY_SOURCE
    "src/*.c"
)

file(
    GLOB_RECURSE LIBRARY_HEADERS
    "include/stb/*.h"
)

add_library(
	third_party STATIC
	${LIBRARY_SOURCE}
    ${LIBRARY_HEADERS}
)

target_include_directories(
    third_party SYSTEM PUBLIC
    "include/"
)

set_target_properties(
	third_party PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG      ${PROJECT_SOURCE_DIR}/debug/lib
	LIBRARY_OUTPUT_DIRECTORY_DEBUG      ${PROJECT_SOURCE_DIR}/debug/lib
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE    ${PROJECT_SOURCE_DIR}/release/lib
	LIBRARY_OUTPUT_DIRECTORY_RELEASE    ${PROJECT_SOURCE_DIR}/release/lib
)