#include "state_storage.hpp"

#include "utility/file_system/path.hpp"
#include "utility/file_system/path_storage.hpp"

constexpr const char* DATA_FOLDER_NAME  = "data";
constexpr const char* COMMAND_FILE_NAME = "exe";

SINGLETON_DEFINITOR(core, StateStorage);

core::StateStorage::StateStorage() noexcept
{
    auto data_path = util::PathStorage::touchFolder(
        util::Path::getRelativeToApp(DATA_FOLDER_NAME));
    auto comm_file =
        util::PathStorage::getFilePath(DATA_FOLDER_NAME, COMMAND_FILE_NAME);
    if (comm_file.has_value())
    {
    }
}

void
core::StateStorage::storeNonstatic(const CommandExtend& a_command,
                                   StateType a_type) noexcept
{
    std::lock_guard<std::mutex> lock(m_storage_mutex);
    switch (a_type)
    {
        case StateType::COMMAND:
            m_base_commands.insert(a_command);
            break;
        case StateType::COMMAND_ARGS:
            m_argument_commands.insert(a_command);
            break;
        case StateType::COMMAND_VARS:
            m_variable_commands.insert(a_command);
            break;
        case StateType::UNIQUE:
            m_unique_commands.emplace_back(a_command);
            break;
    }
}

bool
core::StateStorage::BaseComparator::operator()(
    const CommandExtend& first,
    const CommandExtend& second) const noexcept
{
    return first.value < second.value;
}

bool
core::StateStorage::ArgsComparator::operator()(
    const CommandExtend& first,
    const CommandExtend& second) const noexcept
{
    return commandCompare(first, second, first.arguments, second.arguments);
}

bool
core::StateStorage::VarsComparator::operator()(
    const CommandExtend& first,
    const CommandExtend& second) const noexcept
{
    return commandCompare(first, second, first.variables, second.variables);
}

void
core::StateStorage::applyFile(const std::string& a_path_name) const noexcept
{
    return commandCompare(first, second, first.variables, second.variables);
}
