#ifndef ENGINE_APP_INFO_HPP
#define ENGINE_APP_INFO_HPP

#include "engine/util/Version.hpp"

#include <string>

namespace engine
{

class AppInfo
{
public:
    AppInfo(const std::string &appName, const Version &appVersion, const std::string &appCopyright);

    const std::string &getAppName() const;
    const Version &getAppVersion() const;
    const std::string &getAppCopyright() const;

private:
    std::string m_appName;
    Version m_appVersion;
    std::string m_appCopyright;
};

} // namespace engine

#endif // ENGINE_APP_INFO_HPP
