#include "file_router.hpp"

#include "post/post_router.hpp"

std::unordered_map<std::string, decltype(&file::File::dmpParser)>
    file::FileRouter::mRouter = {
        {"dmp",  &file::File::dmpParser },
        {"data", &file::File::dataParser}
};

file::FileDataArray
file::FileRouter::process(const std::string& aFileName) noexcept
{
    int indx = aFileName.size() - 1;
    while (aFileName[indx] != '.') --indx;
    std::string extension = aFileName.substr(indx + 1);

    auto it = mRouter.find(extension);
    std::unordered_map<std::string, FileData> result;
    if (it != mRouter.end()) result = it->second(aFileName);
    return result;
}
