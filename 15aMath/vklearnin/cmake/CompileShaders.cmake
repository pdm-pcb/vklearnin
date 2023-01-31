# Thanks to diapir for this one. Slightly modified for my own uses:
# https://stackoverflow.com/a/60472877/1464937

find_package(Vulkan REQUIRED)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

cmake_policy(SET CMP0116 NEW)

function(compile_shader target)
    cmake_parse_arguments(
        PARSE_ARGV 1 arg
        ""
        "ENV"
        "SOURCES"
    )
    foreach(source ${arg_SOURCES})
        if(CMAKE_BUILD_TYPE MATCHES "Debug")
            set(output_filename "${source}-debug.spv")
            set(shader_optimization "-O0")
            set(shader_debug "-g")
        elseif(CMAKE_BUILD_TYPE MATCHES "Release")
            set(output_filename "${source}-release.spv")
            set(shader_optimization "-O")
            set(shader_debug "")
        else()
            message(FATAL_ERROR "CMake build type ${CMAKE_BUILD_TYPE} unknown.")
        endif()
        add_custom_command(
            OUTPUT ${output_filename}
            DEPENDS ${source}
            DEPFILE ${source}.d
            COMMAND
                ${glslc_executable}
                $<$<BOOL:${arg_ENV}>:--target-env=${arg_ENV}>
                -mfmt=bin
                -MD -MF ${source}.d
                ${shader_debug}
                -o ${output_filename}
                ${source}
        )

        target_sources(
            ${target} PRIVATE
            ${source}
            ${output_filename}
        )
    endforeach()
endfunction()