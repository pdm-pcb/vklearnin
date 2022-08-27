cmake_minimum_required(VERSION 3.24)

project(
    "Vulkan Learnin'"
    VERSION 0.0.1
    LANGUAGES CXX C
)

add_executable(
	demo
    "main.cpp"
)

target_link_libraries(
    demo PRIVATE
    vklearnin
)

add_subdirectory(vklearnin)
add_subdirectory(third_party)

set_target_properties(
	demo PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
    PCH_WARN_INVALID OFF
	EXECUTABLE_OUTPUT_DIRECTORY_DEBUG   ${PROJECT_SOURCE_DIR}/debug/bin
	EXECUTABLE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/release/bin
)