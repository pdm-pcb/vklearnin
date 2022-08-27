include(FetchContent)

message(NOTICE "Fetch fmt")
FetchContent_Declare(
    fmt 8.1.1...
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG c4ee726532178e556d923372f29163bd206d7732 # v9.0.0 | 2022-07-04
    FIND_PACKAGE_ARGS
)