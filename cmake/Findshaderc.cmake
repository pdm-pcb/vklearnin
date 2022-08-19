include(FetchContent)

message(NOTICE "Fetch spirv-headers")
FetchContent_Declare(
  spirv-headers
  GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Headers.git
  GIT_TAG b2a156e1c0434bc8c99aaebba1c7be98be7ac580 # sdk-1.3.216.0 | 2022-05-31
)

set(SPIRV_HEADERS_SKIP_EXAMPLES ON CACHE BOOL "" FORCE)
set(SPIRV_HEADERS_SKIP_INSTALL  ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(spirv-headers)

message(NOTICE "Fetch spirv-tools")
FetchContent_Declare(
  spirv-tools
  GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Tools.git
  GIT_TAG 7826e1941eab1aa66fbe84c48b95921bff402a96 # 2022.2 | 2022-04-01
)
FetchContent_MakeAvailable(spirv-tools)

message(NOTICE "Fetch glslang")
FetchContent_Declare(
  glslang
  GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
  GIT_TAG 73c9630da979017b2f7e19c6549e2bdb93d9b238 # 11.11.0 | 2022-08-12
)
FetchContent_MakeAvailable(glslang)

message(NOTICE "Fetch shaderc")
FetchContent_Declare(
    shaderc
    GIT_REPOSITORY https://github.com/google/shaderc.git
    GIT_TAG 551f106dc64a1cacb298cc2f05613d142b53302f # v2022.2 | 2022-08-12
)

set(SHADERC_SKIP_INSTALL ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_TESTS   ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(shaderc)

set_target_properties(
    shaderc PROPERTIES
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
    $<TARGET_PROPERTY:shaderc,INTERFACE_INCLUDE_DIRECTORIES>
)