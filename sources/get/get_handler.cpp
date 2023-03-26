#include "get_handler.hpp"

#include <ranges>

#include "get_router.hpp"

crow::json::wvalue
get::GetHandler::mainGet(const std::string& aRequest,
                         const std::string& aCondition) noexcept
{
    crow::json::wvalue result;

    data::DataRequest req(aRequest, aCondition);
    data::DatabaseQuery dbq(data::DatabaseQuery::UserType::USER);
    for (auto& i : req)
    {
        dbq.handSelect(i);

        std::vector<crow::json::wvalue> temp;
        for (auto& j : i)
        {
            temp.emplace_back(
                get::GetRouter::basicRouter(j.trueName, j.rowNumbers, dbq));
        }
        dbq.handClose();

        for (size_t j = temp.size() - 1; j >= 1; --j)
        {
            auto& curName1   = i[j].jsonName;
            auto& curName2   = i[j].trueName;
            auto& targetNum  = i[j].parentNum;
            auto& targetName = i[targetNum].trueName;

            for (size_t k = 0; k < temp[targetNum][targetName].size(); ++k)
            {
                temp[targetNum][targetName][k][curName1] =
                    std::move(temp[j][curName2][k]);
            }
        }

        result = std::move(temp[0]);
    }

    return result;
}