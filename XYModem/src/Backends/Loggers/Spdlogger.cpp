#include "Loggers/Spdlogger.h"
#include <cassert>

namespace xymodem
{

Spdlogger::Spdlogger (LogLevel logLevel, const logType& pathToLogFile)
{
    initialiseLogger (pathToLogFile, logLevel);
}

void Spdlogger::initialiseLogger (const logType& pathToLogFile,
                                  const LogLevel logLevel)
{
    try
    {
        if (!pathToLogFile.empty())
        {
            auto console_sink =
                std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level (spdlog::level::info);
            auto file_sink =
                std::make_shared<spdlog::sinks::basic_file_sink_mt> (
                    pathToLogFile, true);
            file_sink->set_level (spdlog::level::debug);
            logger = std::make_shared<spdlog::logger> (
                "multi_sink",
                std::initializer_list<spdlog::sink_ptr> (
                    { console_sink, file_sink }));
        }
        else
        {
            auto console_sink =
                std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level (spdlog::level::debug);
            logger = std::make_shared<spdlog::logger> (
                "mono_sink",
                std::initializer_list<spdlog::sink_ptr> ({ console_sink }));
        }
        logger->set_level (static_cast<spdlog::level::level_enum> (logLevel));
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        logger->warn ("Log init failed : {}", ex.what());
    }
}

void Spdlogger::logMessage (std::string_view message, LogLevel level) const
{
    logger->log (static_cast<spdlog::level::level_enum> (level), message);
}
} // namespace xymodem
