#pragma once
#include "spdlog/spdlog.h"
#include "utf8.h"
#include <functional>
#include <string_view>
#include <unordered_map>
#ifdef _WIN32
// clang-format off
#include "windows.h"
#include "shellapi.h"
// clang-format on
#endif

class CLIParser
{
public:
    CLIParser() = default;
    explicit CLIParser (std::unordered_map<std::string_view, std::function<void (std::string_view)>> t_commands);

    /** Parse all arguments given to the command line. When a name in the
     * command line matches a key in the map, the corresponding function will be
     * executed and given the value just after it (if there is one or the next
     * value is not another command).
     * @param argc number of space-separated values in the command line
     * @param argv array of pointers to the space-separated values
     */
    void parse (int argc, char* argv[]);

    /** Each function will be executed if the corresponding key is found in the
     * command line arguments, and given the corresponding parameter
     */
    void setCommands (std::unordered_map<std::string_view, std::function<void (std::string_view)>>&& t_commands);
    /** Each function will be executed if the corresponding key is found in the
     * command line arguments, and given the corresponding parameter
     */
    void setCommands (const std::unordered_map<std::string_view, std::function<void (std::string_view)>>& t_commands);

private:
    std::unordered_map<std::string_view, std::function<void (std::string_view)>> m_commands;
};
