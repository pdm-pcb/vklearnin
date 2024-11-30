function(add_warning_flags)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(
            "-Wall"     # Enable most warnings
            "-Wextra"   # ...and a few more
            "-pedantic" # ...and language compliance checks
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        add_compile_options(
            "/W4" # All warnings not disabled by default
        )
    endif()
endfunction()
