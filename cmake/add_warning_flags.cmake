function(add_warning_flags)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(
            "-Wall"     # Enable most warnings
            "-Wextra"   # ...and a few more
            "-pedantic" # ...and language compliance checks
        )

        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            add_compile_options(
                "-Weverything"      # Clang supports a true "-Wall"
                "-Wno-c++98-compat" # ...but we don't care about C++98
                "-Wno-c++98-compat-pedantic"
                "-Wno-c++20-compat" # And we're targeting C++20
                "-Wno-newline-eof"  # I'm willing to accept this risk
                "-Wno-global-constructors"   # There are some statics I like
                "-Wno-exit-time-destructors" # Same as above
                "-Wno-padded" # Dont worry about padded structs etc
                "-Wno-weak-vtables" # Not quite sure what to do about this one
            )

            if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 16)
                add_compile_options(
                    "-Wno-unsafe-buffer-usage" # A fine reminder, but no thanks
                )
            endif()

        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        add_compile_options(
            "/W4" # All warnings not disabled by default
        )
    endif()
endfunction()
