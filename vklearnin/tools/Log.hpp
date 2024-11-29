/**
 * @file Log.hpp
 * @brief A wrapper class for spdlog with a few customizations.
 */

#ifndef VKLEARNIN_TOOLS_CONSOLELOG_HPP
#define VKLEARNIN_TOOLS_CONSOLELOG_HPP

#include "vklearnin/pch.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace vkl {

class Log final {
public:
    enum class Level : uint8_t {
        LOG_TRACE,
        LOG_INFO,
        LOG_WARN,
        LOG_ERROR,
        LOG_CRITICAL
    };

    static void set_level(Level log_level);

    template<typename ...T>
    static void trace(fmt::format_string<T...> fmt, T&& ...args);

    template<typename ...T>
    static void info(fmt::format_string<T...> fmt, T&& ...args);

    template<typename ...T>
    static void warn(fmt::format_string<T...> fmt, T&& ...args);

    template<typename ...T>
    static void error(fmt::format_string<T...> fmt, T&& ...args);

    template<typename ...T>
    static void critical(fmt::format_string<T...> fmt, T&& ...args);

    Log() = delete;

private:
    static spdlog::logger *_logger;
	static void _init();
};

// =============================================================================
// Implementations

template<typename ...T>
void Log::trace(fmt::format_string<T...> fstring, T&& ...args) {
    _init();
    _logger->trace(fstring, std::forward<T>(args)...);
}

template<typename ...T>
void Log::info(fmt::format_string<T...> fstring, T&& ...args) {
    _init();
    _logger->info(fstring, std::forward<T>(args)...);
}

template<typename ...T>
void Log::warn(fmt::format_string<T...> fstring, T&& ...args) {
    _init();
    _logger->warn(fstring, std::forward<T>(args)...);
}

template<typename ...T>
void Log::error(fmt::format_string<T...> fstring, T&& ...args) {
    _init();
    _logger->error(fstring, std::forward<T>(args)...);
}

template<typename ...T>
void Log::critical(fmt::format_string<T...> fstring, T&& ...args) {
    _init();

    auto const errmsg = fmt::format(fstring, std::forward<T>(args)...);

    _logger->critical(errmsg);
    assert(false);
}

} // namespace vkl

#endif // VKLEARNIN_TOOLS_CONSOLELOG_HPP