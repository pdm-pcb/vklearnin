cmake_minimum_required(VERSION 3.20) # This minimum requirement allows us to not
                                     # worry whether CMake is automatically
                                     # adding the RTTI enable flag to an MSVC
                                     # build

project(
    vklearnin
    VERSION 0.1.0
    DESCRIPTION "Exploring real time graphics."
    LANGUAGES CXX C
)

add_subdirectory(vklearnin)
add_subdirectory(demo)
add_subdirectory(third_party)