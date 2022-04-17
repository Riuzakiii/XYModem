#pragma once

#include "Files/File.h"
#include "Devices/SerialHandler.h"
#include "Loggers/Logger.h"
#include "Loggers/Spdlogger.h"
#include "Senders/YModemSender.h"
#include "Files/DesktopFile.h" // note : some strange error if placed before YModemSender, interfere with crc_cpp for some semi colon reason
