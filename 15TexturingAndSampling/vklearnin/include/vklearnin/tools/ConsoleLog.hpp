#ifndef VKLEARNIN_TOOLS_CONSOLELOG_HPP
#define VKLEARNIN_TOOLS_CONSOLELOG_HPP

// This directive lets spdlog know we want to show every type of message, up to
// and including traces.
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h> // definitely want color, right?

#include <cassert>

namespace vkl {

class ConsoleLog final {
public:
    static void init() {
        spdlog::set_level(spdlog::level::trace);
        // The format string requests color, time with milliseconds, thread ID,
        // and the name of the function in which the logging macro was expanded
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
// I like to have anything I mark as critical immediately halt execution
#define CONSOLE_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__); assert(false)

} // namespace vkl
#endif // VKLEARNIN_TOOLS_CONSOLELOG_HPP