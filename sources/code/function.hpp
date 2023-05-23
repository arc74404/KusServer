#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <fstream>
#include <string>

namespace code
{
class Function
{
public:
    Function() noexcept;

    void setNamespace(const std::string& aNamespace) noexcept;
    void setClass(const std::string& aClass) noexcept;

    void makeStatic() noexcept;
    void makeVariadic() noexcept;

    void setTemplate(const std::string& aTemplate) noexcept;
    void setReturnType(const std::string& aType) noexcept;
    void setName(const std::string& aName) noexcept;
    void setArguments(const std::string& aArguments) noexcept;
    void setBody(const std::string& aBody) noexcept;

    void outputToHpp(std::ofstream& aOut) const noexcept;
    void outputToCpp(std::ofstream& aOut) const noexcept;

    void makeRouter(std::string aMapName, std::string aExecExpr) noexcept;

private:
    bool mIsStatic;
    bool mIsVariadic;

    std::string mNamespace;
    std::string mClass;

    std::string mTemplate;
    std::string mType;
    std::string mName;
    std::string mArguments;
    std::string mBody;
};

} // namespace code

#endif // !FUNCTION_HPP
