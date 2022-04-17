#pragma once
#include <string_view>

namespace xymodem
{
// 0(trace), 1(debug), 2(info), 3(warn), 4(error), 5(level
//      * critical), 6(off)
class Logger
{
public:

    enum class LogLevel
    {
        trace,
        debug,
        info,
        warning,
        error,
        critical,
        off
    };

    Logger() = default;
    virtual void logMessage(std::string_view message, LogLevel level = LogLevel::info) const;
    virtual void debug(std::string_view message) const;
    virtual void warn(std::string_view message) const;
    virtual void info(std::string_view message) const;
    virtual void error(std::string_view message) const;
};
}
