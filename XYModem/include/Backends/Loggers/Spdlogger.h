#pragma once
#include "Loggers/Logger.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <string_view>

namespace xymodem
{
using logType = spdlog::filename_t;

class Spdlogger : public Logger
{
public:
    Spdlogger (LogLevel logLevel = LogLevel::info,
               const logType& pathToLogFile = {});
    void logMessage (std::string_view message,
                     LogLevel level = LogLevel::info) const override;

private:
    std::shared_ptr<spdlog::logger> logger = spdlog::default_logger();
    void initialiseLogger (const logType& pathToLogFile, LogLevel logLevel);
};
} // namespace xymodem
