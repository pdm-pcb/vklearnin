include(FetchContent)

# ------------------------------------------------------------------------------
# spdlog -----------------------------------------------------------------------
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG ad0e89cbfb4d0c1ce4d097e134eb7be67baebb36 # 2022-11-02
    FIND_PACKAGE_ARGS 1.11.0
)