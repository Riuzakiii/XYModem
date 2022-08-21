// Copyright 2022 Riuzakiii

#include "Files/DesktopFile.h"
#include <cassert>

namespace xymodem
{
DesktopFile::DesktopFile (const ghc::filesystem::path& filePath) : fileStream (std::make_unique<ghc::filesystem::fstream>()), filePath (filePath)
{
    if (DesktopFile::exists())
    {
        filename = filePath.filename().string();
        filesize = static_cast<std::intmax_t> (ghc::filesystem::file_size (filePath));
        lastModificationDate = std::chrono::duration_cast<std::chrono::seconds> (ghc::filesystem::last_write_time (filePath).time_since_epoch()).count();
    }
}

std::string DesktopFile::getNextFileBlock (const std::intmax_t blockSizeBytes)
{
    assert (isOpened());
    std::string block;
    fileStream->seekg (readPtr);
    for (int i = 0; i != blockSizeBytes; ++i)
    {
        char byte = '\0';
        fileStream->read (&byte, 1);
        block.push_back (byte);
    }
    readPtr += blockSizeBytes;
    return block;
}

void DesktopFile::append (const std::vector<uint8_t>& data, bool overwrite)
{
    assert (isOpened());
    static_assert (sizeof (char) == sizeof (uint8_t));
    fileStream->seekp (0, fileStream->end);
    fileStream->write (reinterpret_cast<const char*> (data.data()), data.size());
}

bool DesktopFile::create()
{
    if (!DesktopFile::exists() && !isOpened())
    {
        fileStream = std::make_unique<ghc::filesystem::fstream>();
        fileStream->open (filePath, std::ios::out);
        auto status = fileStream->good();
        fileStream.reset (nullptr);
        return status;
    }
    return false;
}

bool DesktopFile::destroy()
{
    if (exists())
    {
        fileStream.reset (nullptr);
        return ghc::filesystem::remove (filePath);
    }
    return false;
}

bool DesktopFile::exists() const { return ghc::filesystem::exists (filePath); }

bool DesktopFile::isOpened() const
{
    if (fileStream != nullptr)
    {
        return fileStream->is_open();
    }
    return false;
}

void DesktopFile::open()
{
    if (DesktopFile::exists())
    {
        fileStream = std::make_unique<ghc::filesystem::fstream>();
        fileStream->open (filePath, std::ios::binary | std::ios::out | std::ios::in);
    }
}

void DesktopFile::erase()
{
    if (isOpened())
    {
        ghc::filesystem::resize_file (filePath, 0);
        readPtr = 0;
    }
}

void DesktopFile::flush()
{
    if (isOpened())
    {
        fileStream->flush();
    }
}

std::string DesktopFile::getFilename() const
{
    if (DesktopFile::exists())
    {
        return filePath.filename().string();
    }
    return {};
}

std::intmax_t DesktopFile::getFilesize() const
{
    if (DesktopFile::exists())
    {
        return static_cast<std::intmax_t> (ghc::filesystem::file_size (filePath));
    }
    return {};
}

std::intmax_t DesktopFile::getLastModificationDate() const
{
    if (DesktopFile::exists())
    {
        return std::chrono::duration_cast<std::chrono::seconds> (ghc::filesystem::last_write_time (filePath).time_since_epoch()).count();
    }
    return {};
}

} // namespace xymodem
