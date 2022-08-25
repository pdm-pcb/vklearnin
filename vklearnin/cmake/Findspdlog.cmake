include(FetchContent)

message(NOTICE "Fetch spdlog")
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG 76fb40d95455f249bd70824ecfcae7a8f0930fa3 # v1.10.0 | 2022-04-04
)
FetchContent_MakeAvailable(spdlog)

set_target_properties(
    spdlog PROPERTIES
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
    $<TARGET_PROPERTY:spdlog,INTERFACE_INCLUDE_DIRECTORIES>
)

# get_target_property(
#     IID spdlog
#     INTERFACE_INCLUDE_DIRECTORIES
# )
# set_target_properties(
#     spdlog PROPERTIES
#     INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${IID}"
# )