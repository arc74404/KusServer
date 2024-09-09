#include "file.hpp"

#include <fstream>
#include <iostream>

std::string
util::File::read(std::string_view a_path) noexcept
{
    std::string result;
    std::ifstream file(a_path.data());
    std::getline(file, result, '\0');
    return result;
}
