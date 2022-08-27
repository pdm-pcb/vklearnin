include(FetchContent)

message(NOTICE "Fetch spdlog")
FetchContent_Declare(
    spdlog 1.10.0...
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG 76fb40d95455f249bd70824ecfcae7a8f0930fa3 # v1.10.0 | 2022-04-04
    FIND_PACKAGE_ARGS
)