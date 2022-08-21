// Copyright 2022 Riuzakiii

#include "Files/File.h"

namespace xymodem
{

std::string File::getFilename() const { return filename; }

std::intmax_t File::getFilesize() const { return filesize; }

std::intmax_t File::getLastModificationDate() const { return lastModificationDate; }

void File::erase() {}

void File::flush() {}

void File::open() {}

void File::close() {}

bool File::destroy() { return false; }
} // namespace xymodem
