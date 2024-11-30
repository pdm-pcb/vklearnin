add_executable(${PROJECT_NAME})

configure_file("version.hpp.in" "${CMAKE_CURRENT_SOURCE_DIR}/version.hpp")

target_precompile_headers(${PROJECT_NAME} PRIVATE "pch.hpp")

file(GLOB_RECURSE sources "*.cpp")
file(GLOB_RECURSE headers "*.hpp")

target_sources(${PROJECT_NAME} PRIVATE ${sources} PRIVATE ${headers})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        Vulkan::Vulkan
        fmt
        spdlog::spdlog
        glfw
        glm::glm
        spirv-reflect-static
        stb
)

file(
    GLOB shader_sources
    "${CMAKE_PROJECT_DIR}/assets/shaders/*.vert"
    "${CMAKE_PROJECT_DIR}/assets/shaders/*.frag"
    "${CMAKE_PROJECT_DIR}/assets/shaders/*.comp"
)

include("compile_shaders")
compile_shader(${PROJECT_NAME} "${shader_sources}")