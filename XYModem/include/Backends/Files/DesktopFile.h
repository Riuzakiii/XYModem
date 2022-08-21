#pragma once
#include "Files/File.h"
//#include <filesystem> Unfortunately only supported on most recent OS versions
#include "ghc/filesystem.hpp" //Workaround, when filesystem is more widely used just change ghc::filesystem for std::filesystem

namespace xymodem
{
class DesktopFile : public File
{
public:
    explicit DesktopFile (const ghc::filesystem::path& filePath);

    std::string getNextFileBlock (const std::intmax_t blockSizeBytes) override;
    void append (const std::vector<uint8_t>& data, bool overwrite) override;
    bool create() override;
    bool destroy() override;
    bool exists() const override;
    void erase() override;
    [[nodiscard]] bool isOpened() const override;
    void open() override;
    void flush();

    [[nodiscard]] std::string getFilename() const override;
    [[nodiscard]] std::intmax_t getFilesize() const override;
    [[nodiscard]] std::intmax_t getLastModificationDate() const override;

private:
    std::unique_ptr<ghc::filesystem::fstream> fileStream = nullptr;
    ghc::filesystem::path filePath;
    ghc::filesystem::fstream::pos_type readPtr = 0;
};
} // namespace xymodem
