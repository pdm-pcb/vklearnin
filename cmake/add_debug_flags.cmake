function(add_debug_flags)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        if(CMAKE_BUILD_TYPE MATCHES "Debug")
            add_compile_options(
                "-Og"    # Debug-safe optimization
                "-ggdb3" # All the gdb symbol data
            #     "-fsanitize=address"
            # )
            # target_link_options(
            #     ${target_name} PUBLIC
            #     "-fsanitize=address"
            )
        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        if(CMAKE_BUILD_TYPE MATCHES "Debug")
            add_compile_options(
                "/Od"   # Optimization disabled
                "/Zi"   # Generate .pdb files: implies /debug
                "/JMC"  # Just-My-Code: only step over user code
                "/RTCs" # Runtime checks for stack frame behaviors
                "/RTCu" # Runtime checks for uninitialized variables
                "/GS"   # Buffer overrun checks
                "/sdl"  # Compile and runtime security checks
                "/diagnostics:column" # Include column in compiler messages
            )

            add_link_options(
                "/DEBUG:FULL" # Move private symbol information into single .pdb
            )
        endif()
    endif()
endfunction()
