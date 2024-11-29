#include "vklearnin/vklearnin.hpp"
#include "vklearnin/tools/Log.hpp"

namespace vkl {

spdlog::logger *Log::_logger = nullptr;

// =============================================================================
void Log::_init() {
    // Ensure the logger is only set up once
    static std::once_flag initialized;

    std::call_once(initialized, [] {
        // Grab a threadsafe logger that supports colorized output
        _logger = spdlog::stdout_color_mt("vkl_logger").get();

        // Print with color, time with milliseconds, and thread ID
        _logger->set_pattern("%^[%T.%e][%t]: %v%$");

        // Default to everything
        _logger->set_level(spdlog::level::trace);

        auto const fmt_major = FMT_VERSION / 10000;
        auto const fmt_minor = (FMT_VERSION - fmt_major * 10000) / 100;
        auto const fmt_patch = (FMT_VERSION - fmt_major * 10000 - fmt_minor * 100);

        _logger->info(
            "spdlog v{}.{}.{} using libfmt v{}.{}.{}",
            SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH,
            fmt_major, fmt_minor, fmt_patch
        );
    });
}

// =============================================================================
void Log::set_level(Level log_level) {
    _init(); // Make sure we're set up

    // Set the severity to print
    switch(log_level) {
        case Level::LOG_TRACE: _logger->set_level(spdlog::level::trace); break;
        case Level::LOG_INFO : _logger->set_level(spdlog::level::info);  break;
        case Level::LOG_WARN : _logger->set_level(spdlog::level::warn);  break;
        case Level::LOG_ERROR: _logger->set_level(spdlog::level::err);   break;
        case Level::LOG_CRITICAL:
            _logger->set_level(spdlog::level::critical);
            break;
    }
}

} // namespace vkl