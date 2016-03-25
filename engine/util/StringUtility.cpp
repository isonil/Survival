#include "StringUtility.hpp"

#include "Math.hpp"

#include <algorithm>

namespace engine
{

bool StringUtility::isAlphaNumeric(const std::string &str)
{
    return std::all_of(str.begin(), str.end(), [](auto ch) { return std::isalnum(ch); });
}

bool StringUtility::isPrintable(const std::string &str)
{
    return std::all_of(str.begin(), str.end(),
                       [](auto ch) { return std::isprint(ch) || std::isspace(ch); });
}

bool StringUtility::noNewlines(const std::string &str)
{
    return std::all_of(str.begin(), str.end(),
                       [](auto ch) { return ch != '\n' && ch != '\r'; });
}

std::string StringUtility::trimTrailingWhitespace(const std::string &str)
{
    const auto &it = std::find_if_not(str.rbegin(), str.rend(), [](auto ch) { return std::isspace(ch); });
    return std::string(str.begin(), it.base());
}

std::string StringUtility::trimLeadingWhitespace(const std::string &str)
{
    const auto &it = std::find_if_not(str.begin(), str.end(), [](auto ch) { return std::isspace(ch); });
    return str.substr(it - str.begin());
}

std::string StringUtility::trimWhitespace(const std::string &str)
{
    auto pred = [](auto ch) { return std::isspace(ch); };
    const auto &first = std::find_if_not(str.begin(), str.end(), pred);
    const auto &last = std::find_if_not(str.rbegin(), str.rend(), pred);

    if(first == str.end())
        return std::string{};

    return std::string(first, last.base());
}

std::string StringUtility::toIntPercentString(float percent)
{
    return std::to_string(Math::roundToInt(percent * 100.f)) + "%";
}

std::string StringUtility::toStringWithSign(int val)
{
    return (val < 0 ? '-' : '+') + std::to_string(val);
}

} // namespace engine
