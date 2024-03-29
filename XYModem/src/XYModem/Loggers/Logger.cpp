﻿// Copyright 2022 Riuzakiii

#include "Loggers/Logger.h"

namespace xymodem
{

void Logger::logMessage (std::string_view /*message*/, LogLevel /*logLevel*/) const {}

void Logger::debug (std::string_view message) const { logMessage (message, LogLevel::debug); }

void Logger::warn (std::string_view message) const { logMessage (message, LogLevel::warning); }

void Logger::info (std::string_view message) const { logMessage (message, LogLevel::info); }

void Logger::error (std::string_view message) const { logMessage (message, LogLevel::error); }
} // namespace xymodem
