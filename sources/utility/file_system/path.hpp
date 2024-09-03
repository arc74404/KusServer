#pragma once

//------------------------------------------------------------------------------

#include <string>
#include <string_view>

#include "core/logging/logging.hpp"

#include "utility/common/holy_trinity.hpp"

static inline constexpr const char PATH_SEPARATOR = '/';

//------------------------------------------------------------------------------

namespace util
{
class Path
{
public:
    HOLY_TRINITY_NO_OBJECT(Path);

public:
    template <typename... Args>
    static std::string combine(Args... args) noexcept
    {
        return combineImpl(std::string_view(args)...);
    }

    static std::string getName(std::string_view a_path,
                               const core::Context& a_context) noexcept;

    static std::string normalizeFolderPath(std::string_view a_path) noexcept;

private:
    template <typename... Args>
    static std::string combineImpl(Args... args) noexcept
    {
        std::string result;
        size_t result_size = (args.size() + ...);
        result.reserve(result_size);
        ((result += args, result.push_back(PATH_SEPARATOR)), ...);
        return result;
    }
};
} // namespace util

#define GET_PATH_LAST_NAME(a_path) util::Path::getName(a_path, LOCAL_CONTEXT)

//------------------------------------------------------------------------------
