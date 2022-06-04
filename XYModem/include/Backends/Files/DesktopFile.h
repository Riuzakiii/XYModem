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
    ~DesktopFile();

    std::string getNextFileBlock (const std::intmax_t blockSizeBytes) override;
    void erase() override;
    [[nodiscard]] bool isOpened() const override;
    void open() override;
    void close() override;

private:
    ghc::filesystem::ifstream dataFile;
    ghc::filesystem::path filePath;
    ghc::filesystem::ifstream fileInputStream;
};
} // namespace xymodem
