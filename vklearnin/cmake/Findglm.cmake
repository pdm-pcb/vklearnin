include(FetchContent)

message(NOTICE "Fetch GLM")
FetchContent_Declare(
    glm 0.9.9.8...
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e # 0.9.9.8 | 2020-04-13
    FIND_PACKAGE_ARGS
)