function(add_release_flags)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        if(CMAKE_BUILD_TYPE MATCHES "Release")
            add_compile_options(
                "-O3" # Optimize for speed, sacrifice size
            )
        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        if(CMAKE_BUILD_TYPE MATCHES "Release")
            add_compile_options(
                "/O2" # Optimize for speed. Implies:
                      # - global optimization
                      # - intrinsics
                      # - "favor fast code"
                      # - frame/pointer omission
                      # - inline function expansion
                      # - eliminate duplicate strings
                      # - function-level linking
            )

            add_link_options(
                "/OPT:NOREF" # Remove unused functions
            )
        endif()
    endif()
endfunction()
