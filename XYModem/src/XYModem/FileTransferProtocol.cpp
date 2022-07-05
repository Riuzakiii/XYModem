// Copyright 2022 Riuzakiii

#include "FileTransferProtocol.h"
#include <cassert> // for assert

namespace xymodem
{
/**
 * GuardConditions
 */

std::intmax_t GuardConditions::get (std::string_view guardName) const { return guardConditions.at (guardName); }

void GuardConditions::set (std::string_view guardName, const std::intmax_t value) { guardConditions.at (guardName) = value; }

void GuardConditions::addGuard (std::string_view guardName, const std::intmax_t value) { guardConditions[guardName] = value; }

void GuardConditions::addGuards (const std::vector<std::pair<std::string_view, std::intmax_t>>& newGuards)
{
    for (const auto& [guardName, value] : newGuards)
    {
        guardConditions.try_emplace (guardName, value);
    }
}

void GuardConditions::clear()
{
    for ([[maybe_unused]] auto& [guard, value] : guardConditions)
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

FileTransferProtocol::FileTransferProtocol (std::shared_ptr<DeviceHandler> deviceHandler_, const unsigned int& currentState_, std::shared_ptr<Logger> logger) :
    deviceHandler (std::move (deviceHandler_)), logger (std::move (logger)), currentState (currentState_)
{
}

void FileTransferProtocol::executeState (const unsigned int /*t_currentState*/, bool /*logHex*/) {}

} // namespace xymodem
