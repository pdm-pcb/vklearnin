include(FetchContent)

message(NOTICE "Fetch GLM")
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e # 0.9.9.8 | 2020-04-13
)
FetchContent_MakeAvailable(glm)

set_target_properties(
    glm PROPERTIES
    BUILD_STATIC_LIBS TRUE

    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
    $<TARGET_PROPERTY:glm,INTERFACE_INCLUDE_DIRECTORIES>
)