#include "CLIParser.h"

using CommandType = std::unordered_map<std::string_view, std::function<void (std::string_view)>>;

CLIParser::CLIParser (CommandType t_commands) : m_commands (std::move (t_commands)) {}

void CLIParser::parse (int argc, [[maybe_unused]] char* argv[])
{
    std::string lastArg;
#ifdef _WIN32
    // SetConsoleCP(CP_UTF8); Will display UTF8 correctly but modifies the user
    // system which is bad practice
    const auto uArgv = CommandLineToArgvW (GetCommandLineW(), &argc);
#else
    std::vector<char*> args;
    std::copy_n (argv, argc, std::back_inserter (args));
#endif
    for (int i = 1; i <= argc; ++i)
    {
        std::string arg;
#ifdef _WIN32
        std::wstring_view warg (L"");
#endif

        if (i < argc)
        {
#ifdef _WIN32
            warg = uArgv[i];
            utf8::utf16to8 (warg.begin(), warg.end(), std::back_inserter (arg));
#else
            arg = args[i];
#endif
        }
        if (!lastArg.empty())
        {
            const auto command = m_commands.find (lastArg);
            if (command != m_commands.end())
            {
                (command->second) (arg);
            }
        }
        lastArg = arg;
    }
}

void CLIParser::setCommands (CommandType&& t_commands) { m_commands = t_commands; }

void CLIParser::setCommands (const CommandType& t_commands) { m_commands = t_commands; }
