set(SPDLOG_FMT_EXTERNAL "ON")

message(STATUS "Fetching spdlog ...")
FetchContent_MakeAvailable(spdlog)
