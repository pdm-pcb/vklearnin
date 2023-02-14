include(FetchContent)

# ------------------------------------------------------------------------------
# spdlog -----------------------------------------------------------------------
message(NOTICE "Fetch spdlog")
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG ad0e89cbfb4d0c1ce4d097e134eb7be67baebb36 # 2022-11-02
    FIND_PACKAGE_ARGS 1.11.0
)
FetchContent_MakeAvailable(spdlog)

# ------------------------------------------------------------------------------
# glm --------------------------------------------------------------------------
message(NOTICE "Fetch glm")
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e # 2020-04-13
    FIND_PACKAGE_ARGS 0.9.9.8
)
FetchContent_MakeAvailable(glm)