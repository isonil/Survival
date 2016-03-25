#ifndef ENGINE_STRING_UTILITY_HPP
#define ENGINE_STRING_UTILITY_HPP

#include <string>

namespace engine
{

class StringUtility
{
public:
    static bool isAlphaNumeric(const std::string &str);
    static bool isPrintable(const std::string &str);
    static bool noNewlines(const std::string &str);
    static std::string trimTrailingWhitespace(const std::string &str);
    static std::string trimLeadingWhitespace(const std::string &str);
    static std::string trimWhitespace(const std::string &str);
    static std::string toIntPercentString(float percent);
    static std::string toStringWithSign(int val);
};

} // namespace engine

#endif // ENGINE_STRING_UTILITY_HPP
