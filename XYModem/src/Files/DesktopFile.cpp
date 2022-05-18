﻿#include "Files/DesktopFile.h"
#include <cassert>

namespace xymodem
{
DesktopFile::DesktopFile(const ghc::filesystem::path& filePath) : filePath(filePath)
{
    filename = filePath.filename ().string ();
    filesize = static_cast<std::intmax_t> (ghc::filesystem::file_size (filePath));
    lastModificationDate =
        ghc::filesystem::last_write_time (filePath)
            .time_since_epoch ()
            .count ();
}

DesktopFile::~DesktopFile()
{
    if(isOpened())
    {
        close();
    }
}

std::string DesktopFile::getNextFileBlock(const std::intmax_t blockSizeBytes)
{
    assert(isOpened());
    std::string block;
    for (int i = 0; i != blockSizeBytes; ++i)
    {
        char t = '\0';
        dataFile.read (&t, 1);
        block.push_back (t);
    }
    return block;
}

bool DesktopFile::isOpened() const
{
    return dataFile.is_open();
}

void DesktopFile::open()
{
    dataFile.open (filePath, std::ios::binary);
}

void DesktopFile::close()
{
    if(dataFile.is_open())
    {
        dataFile.close();
    }
}

void DesktopFile::erase()
{
    dataFile.seekg (0, std::ios::beg);
}
}