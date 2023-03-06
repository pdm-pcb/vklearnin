include(FetchContent)

# ------------------------------------------------------------------------------
# fmtlib -----------------------------------------------------------------------
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG a33701196adfad74917046096bf5a2aa0ab0bb50 # 2022-08-27
    FIND_PACKAGE_ARGS 9.1.0
)

# ------------------------------------------------------------------------------
# spdlog -----------------------------------------------------------------------
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG ad0e89cbfb4d0c1ce4d097e134eb7be67baebb36 # 2022-11-02
    FIND_PACKAGE_ARGS 1.11.0
)

set(SPDLOG_FMT_EXTERNAL "ON" CACHE BOOL "" FORCE)