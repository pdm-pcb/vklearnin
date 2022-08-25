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