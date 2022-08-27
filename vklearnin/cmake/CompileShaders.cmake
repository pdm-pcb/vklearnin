# Thanks to diapir for this one. Slightly modified for my own uses:
# https://stackoverflow.com/a/60472877/1464937

find_package(Vulkan COMPONENTS glslc REQUIRED)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

function(compile_shader target)
    message("Compiling shader sources ${arg_SOURCES}")
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "ENV" "SOURCES")
    foreach(source ${arg_SOURCES})
        add_custom_command(
            OUTPUT ${source}.spv
            DEPENDS ${source}
            DEPFILE ${source}.d
            COMMAND
                ${glslc_executable}
                $<$<BOOL:${arg_ENV}>:--target-env=${arg_ENV}>
                -mfmt=bin
                -MD -MF ${source}.d
                -o ${source}.spv
                ${source}
        )
        target_sources(
            ${target} PRIVATE
            ${source}
            ${source}.spv
        )
    endforeach()
endfunction()