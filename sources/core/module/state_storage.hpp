#pragma once

//--------------------------------------------------------------------------------

#include <mutex>
#include <set>
#include <vector>

#include "core/command/command_extend.hpp"

#include "utility/common/holy_trinity.hpp"

//--------------------------------------------------------------------------------

namespace core
{
class StateStorage
{
public:
    HOLY_TRINITY_SINGLETON(StateStorage);

    enum class StateType
    {
        COMMAND,
        COMMAND_ARGS,
        COMMAND_VARS,
        UNIQUE
    };

    SINGL_VOID_METHOD(store,
                      (const CommandExtend& a_command, StateType a_type));

private:
#define COMPARATOR(name)                                             \
    struct name                                                      \
    {                                                                \
        bool operator()(const CommandExtend& first,                  \
                        const CommandExtend& second) const noexcept; \
    };
    COMPARATOR(BaseComparator);
    COMPARATOR(ArgsComparator);
    COMPARATOR(VarsComparator);
#undef COMPARATOR

    mutable std::mutex m_storage_mutex;
    std::set<CommandExtend, BaseComparator> m_base_commands;
    std::set<CommandExtend, ArgsComparator> m_argument_commands;
    std::set<CommandExtend, VarsComparator> m_variable_commands;
    std::vector<CommandExtend> m_unique_commands;

    StateStorage() noexcept;
    void applyFile(const std::string& a_path_name) noexcept;

    template <typename T1, typename T2>
    static bool commandCompare(const CommandExtend& first,
                               const CommandExtend& second,
                               const T1& first_storage,
                               const T2& second_storage) noexcept
    {
        bool result = false;
        if (first.value != second.value)
        {
            result = first.value < second.value;
        }
        else if (first_storage.size() != second_storage.size())
        {
            result = first_storage.size() < second_storage.size();
        }
        else
        {
            for (auto it1 = first_storage.begin(), it2 = second_storage.begin();
                 it1 != first_storage.end() && it2 != second_storage.end();
                 ++it1, ++it2)
            {
                if (*it1 != *it2)
                {
                    result = *it1 < *it2;
                }
            }
        }
        return result;
    }
};
} // namespace core

//--------------------------------------------------------------------------------
