#ifndef LOG_HPP
#define LOG_HPP

#include <ostream>
#include <vector>

#include "string_data/to_string.hpp"

#include "command.hpp"
#include "command_router.hpp"

namespace base
{

class Log
{
public:
    static void reloadFromSettings() noexcept;
    static std::string applyCommand(const Command& aCommand) noexcept;

    //--------------------------------------------------------------------------------

    template <typename... Args>
    static void writeInfo(Args&&... args) noexcept
    {
        getInstance().writeInfoNonstatic(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void writeWarning(Args&&... args) noexcept
    {
        getInstance().writeWarningNonstatic(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void writeError(Args&&... args) noexcept
    {
        getInstance().writeErrorNonstatic(std::forward<Args>(args)...);
    }

    //--------------------------------------------------------------------------------

private:
    static BasicCommandRouter glCommandRouter;

    enum class PrintStatus
    {
        Nun,
        Info,
        Warning,
        Error
    };
    bool mIsFileOutput;
    PrintStatus mStatus;
    std::ostream* mStreams;

    //--------------------------------------------------------------------------------

    Log() noexcept;
    ~Log();
    void clear() noexcept;
    static Log& getInstance() noexcept;

    //--------------------------------------------------------------------------------

    std::string applyCommandNonstatic(const Command& aCommand) noexcept;
    void setLogType(const std::string& aLogType) noexcept;
    void setOutputType(const std::string& aOutputType) noexcept;

    //--------------------------------------------------------------------------------

    template <typename... Args>
    void write(Args&&... args) noexcept
    {
        std::string str;
        std::vector<std::string> temp;

        (void)(temp.emplace_back(toString(std::forward<Args>(args)), 0), ...);
        for (int i = 0; i < temp.size() - 1; ++i)
        {
            if (!temp[i + 1].empty() && temp[i].back() != '(' &&
                temp[i + 1].front() != ')' && temp[i].back() != '\'' &&
                temp[i + 1].front() != '\'')
            {
                temp[i].push_back(' ');
            }
        }
        for (auto&& i : temp)
        {
            str += std::move(i);
        }

        str += ".\n";

        (*mStreams) << str;
        (*mStreams).flush();
    }

    //--------------------------------------------------------------------------------

    template <typename... Args>
    void writeInfoNonstatic(Args&&... args) noexcept
    {
        if (mStatus == PrintStatus::Info)
        {
            write("Info: ", std::forward<Args>(args)...);
        }
    }

    template <typename... Args>
    void writeWarningNonstatic(Args&&... args) noexcept
    {
        if (mStatus <= PrintStatus::Warning)
        {
            write("->Warning: ", std::forward<Args>(args)...);
        }
    }

    template <typename... Args>
    void writeErrorNonstatic(Args&&... args) noexcept
    {
        if (mStatus <= PrintStatus::Error)
        {
            write("!--->ERROR: ", std::forward<Args>(args)...);
        }
    }

    //--------------------------------------------------------------------------------
};

} // namespace base

#endif // !LOG_HPP
