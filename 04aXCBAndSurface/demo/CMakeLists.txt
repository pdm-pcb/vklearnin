file(
    GLOB_RECURSE DEMO_SOURCE
    "src/*.cpp"
)

file(
    GLOB_RECURSE DEMO_HEADERS
    "include/*.hpp"
)

add_executable(
    demo
	${DEMO_SOURCE}
    ${DEMO_HEADERS}
)

target_include_directories(
	demo PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
	demo PUBLIC
	vklearnin
)

set_target_properties(
	demo PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/debug/bin
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/release/bin
)