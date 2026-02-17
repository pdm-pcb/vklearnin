function(set_target_arch)
    # For x86_64, target Haswell/Excavator or newer
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64|AMD64")
            message(STATUS "Optimizing for x86-64-v3")
            add_compile_options("-march=x86-64-v3")
        elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
            execute_process(
                COMMAND grep Raspberry /proc/cpuinfo
                OUTPUT_VARIABLE CPU_Model
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )

            if("${CPU_Model}" MATCHES "Raspberry Pi 4")
                message(STATUS "Optimizing for Raspberry Pi 4")
                add_compile_options("-march=armv8-a"
                                    "-mcpu=cortex-a72")
            elseif("${CPU_Model}" MATCHES "Raspberry Pi 5")
                message(WARNING "No special optimizations for Raspberry Pi 5, yet")
            else()
                message(WARNING "Unknown CPU architecture ${CMAKE_SYSTEM_PROCESSOR} not targeted.")
            endif()
        else()
            message(WARNING "Unknown CPU architecture ${CMAKE_SYSTEM_PROCESSOR} not targeted.")
        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64")
            message(STATUS "Optimizing for AVX2-capable CPUs")
            add_compile_options("/arch:AVX2")
            else()
                message(WARNING "Unknown CPU architecture ${CMAKE_SYSTEM_PROCESSOR} not targeted.")
        endif()
    endif()
endfunction()
