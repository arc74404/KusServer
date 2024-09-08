#include "path.hpp"

#include "path_storage.hpp"

std::string
util::Path::getName(std::string_view a_path,
                    const core::Context& a_context) noexcept
{
    std::string result;
    result.reserve(a_path.size());
    int start = a_path.size() - 1;
    if (a_path[start] == PATH_SEPARATOR)
    {
        --start;
    }
    for (int i = start; i >= 0; --i)
    {
        if (a_path[i] != PATH_SEPARATOR)
        {
            result.push_back(a_path[i]);
        }
        else
        {
            break;
        }
    }
    if (result.empty())
    {
        CONTEXT_ERROR(a_context,
                      "There is no file or folder at the end of the path: '%s'",
                      a_path);
    }
    return result;
}

std::string
util::Path::normalizeFolderPath(std::string_view a_path) noexcept
{
    std::string result(a_path);
    if (!result.empty() && result.back() != PATH_SEPARATOR)
    {
        result.push_back(PATH_SEPARATOR);
    }
    return result;
}

std::string
util::Path::getRelativeToApp(std::string_view a_path) noexcept
{
    std::string result;
    DECLARE_LOCAL_CONTEXT;
    auto app_path = PathStorage::getFolderPath(APP_PATH_NAME, &context_local);
    result        = combine(app_path.value(), a_path);
    return result;
}
