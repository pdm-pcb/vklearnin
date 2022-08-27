#ifndef VKLEARNIN_CONSOLELOG_HPP
#define VKLEARNIN_CONSOLELOG_HPP

#define SPDLOG_FMT_EXTERNAL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <spdlog/fmt/xchar.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <cassert>

class ConsoleLog {
public:
    static void init() {
        spdlog::set_level(spdlog::level::trace);
        spdlog::set_pattern("%^[%T.%e][ %t ][%!()]: %v%$");
        SPDLOG_INFO("spdlog v{}.{}.{}", SPDLOG_VER_MAJOR,
                                        SPDLOG_VER_MINOR,
                                        SPDLOG_VER_PATCH);
    }

    ConsoleLog() = delete;
};

#define CONSOLE_TRACE(...)    SPDLOG_TRACE(__VA_ARGS__)
#define CONSOLE_INFO(...)     SPDLOG_INFO(__VA_ARGS__)
#define CONSOLE_WARN(...)     SPDLOG_WARN(__VA_ARGS__)
#define CONSOLE_ERROR(...)    SPDLOG_ERROR(__VA_ARGS__)
#define CONSOLE_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__); assert(false)

#endif // VKLEARNIN_CONSOLELOG_HPP