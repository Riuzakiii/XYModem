#include "FileTransferProtocol.h"
#include <cassert>

namespace xymodem
{
/**
 * GuardConditions
 */

std::intmax_t GuardConditions::get (std::string_view guardName) const
{
    return guardConditions.at (guardName);
}

void GuardConditions::set (std::string_view guardName,
                           const std::intmax_t value)
{
    guardConditions.at (guardName) = value;
}

void GuardConditions::addGuard (std::string_view guardName,
                                const std::intmax_t value)
{
    guardConditions[guardName] = value;
}

void GuardConditions::addGuards (
    const std::vector<std::pair<std::string_view, std::intmax_t>>& newGuards)
{
    for (const auto& [guardName, value] : newGuards)
    {
        guardConditions.try_emplace (guardName, value);
    }
}

void GuardConditions::clear ()
{
    for (auto& [guard, value] : guardConditions)
    {
        value = 0;
    }
}

void GuardConditions::inc (std::string_view guardName)
{
    assert (guardConditions.at (guardName) < INTMAX_MAX);
    ++guardConditions.at (guardName);
}

void GuardConditions::dec (std::string_view guardName)
{
    assert (guardConditions.at (guardName) > INTMAX_MIN);
    --guardConditions.at (guardName);
}


/**
 *  FileTransferProtocol
 */

FileTransferProtocol::FileTransferProtocol (DeviceHandler& deviceHandler_,
                                            const unsigned int& currentState_,
                                            const logType pathToLogFile,
                                            const int logLevel)
    : deviceHandler (deviceHandler_), currentState (currentState_)
{
    initialiseLogger (pathToLogFile, logLevel);
}

FileTransferProtocol::FileTransferProtocol (
    DeviceHandler& deviceHandler_,
    const unsigned int& currentState_,
    std::shared_ptr<spdlog::logger> logger)
    : logger (std::move (logger)),
      deviceHandler (deviceHandler_),
      currentState (currentState_)
{
}

void FileTransferProtocol::setFileInfos (
    const ghc::filesystem::path& fileAbsolutePath)
{
    fileInfos.filePath = fileAbsolutePath;
    fileInfos.fileName = fileInfos.filePath.filename ().string ();
    fileInfos.fileSize = static_cast<std::intmax_t> (
        ghc::filesystem::file_size (fileAbsolutePath));
    fileInfos.lastModificationDate =
        ghc::filesystem::last_write_time (fileAbsolutePath)
            .time_since_epoch ()
            .count ();
    logger->info ("Set file information");
    logger->info ("Filename {}", fileInfos.fileName);
    logger->info ("Filepath {}", fileInfos.filePath.string ());
    logger->info ("Filesize {} MB",
                  static_cast<float> (fileInfos.fileSize) / (1024.f * 1024.f));
    logger->info ("Last modification {} since epoch",
                  fileInfos.lastModificationDate);
}

std::string
FileTransferProtocol::getNextFileBlock (std::ifstream& file,
                                        const std::intmax_t blockSize)
{
    std::string block;
    for (int i = 0; i != blockSize; ++i)
    {
        char t = '\0';
        file.read (&t, 1);
        block.push_back (t);
    }
    return block;
}

void FileTransferProtocol::executeState (const unsigned int /*t_currentState*/,
                                         bool /*logHex*/)
{
}

void FileTransferProtocol::initialiseLogger (const logType pathToLogFile, const int logLevel)
{
    try
    {
        if (!pathToLogFile.empty ())
        {
            auto console_sink =
                std::make_shared<spdlog::sinks::stdout_color_sink_mt> ();
            console_sink->set_level (spdlog::level::info);
            auto file_sink =
                std::make_shared<spdlog::sinks::basic_file_sink_mt> (
                    pathToLogFile, true);
            file_sink->set_level (spdlog::level::debug);
            logger = std::make_shared<spdlog::logger> (
                "multi_sink",
                std::initializer_list<spdlog::sink_ptr> (
                    {console_sink, file_sink}));
        }
        else
        {
            auto console_sink =
                std::make_shared<spdlog::sinks::stdout_color_sink_mt> ();
            console_sink->set_level (spdlog::level::debug);
            logger = std::make_shared<spdlog::logger> (
                "mono_sink",
                std::initializer_list<spdlog::sink_ptr> ({console_sink}));
        }
        assert (0 <= logLevel && logLevel <= 6);
        logger->set_level (
            static_cast<spdlog::level::level_enum> (logLevel));
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        logger->warn ("Log init failed : {}", ex.what ());
    }
}

} // namespace xymodem
