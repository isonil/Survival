#include "AppInfo.hpp"

namespace engine
{

AppInfo::AppInfo(const std::string &appName, const Version &appVersion, const std::string &appCopyright)
    : m_appName{appName}, m_appVersion{appVersion}, m_appCopyright{appCopyright}
{
}

const std::string &AppInfo::getAppName() const
{
    return m_appName;
}

const Version &AppInfo::getAppVersion() const
{
    return m_appVersion;
}

const std::string &AppInfo::getAppCopyright() const
{
    return m_appCopyright;
}

} // namespace engine
