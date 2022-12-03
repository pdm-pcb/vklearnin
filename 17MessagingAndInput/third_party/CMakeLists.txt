file(
    GLOB_RECURSE THIRD_PARTY_SOURCE
    "*.cpp"
    "*.c"
)

add_library(
    third_party
	${THIRD_PARTY_SOURCE}
    ${THIRD_PARTY_HEADERS}
)

target_include_directories(
    third_party SYSTEM PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}"
)

set_target_properties(
	third_party PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG      ${PROJECT_SOURCE_DIR}/debug/lib
	LIBRARY_OUTPUT_DIRECTORY_DEBUG      ${PROJECT_SOURCE_DIR}/debug/lib
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE    ${PROJECT_SOURCE_DIR}/release/lib
	LIBRARY_OUTPUT_DIRECTORY_RELEASE    ${PROJECT_SOURCE_DIR}/release/lib
)