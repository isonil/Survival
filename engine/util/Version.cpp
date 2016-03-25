#include "Version.hpp"

#include <cstdio>
#include <sstream>

namespace engine
{

Version::Version()
    : m_major{}, m_minor{}, m_revision{}
{
}

Version::Version(int major, int minor, int revision)
    : m_major{major}, m_minor{minor}, m_revision{revision}
{
}

bool Version::operator == (const Version &ver) const
{
    return m_major == ver.m_major &&
           m_minor == ver.m_minor &&
           m_revision == ver.m_revision;
}

bool Version::operator != (const Version &ver) const
{
    return !(*this == ver);
}

bool Version::operator < (const Version &ver) const
{
    if(m_major == ver.m_major) {
        if(m_minor == ver.m_minor) {
            return m_revision < ver.m_revision;
        }
        else return m_minor < ver.m_minor;
    }
    else return m_major < ver.m_major;
}

bool Version::operator <= (const Version &ver) const
{
    return *this < ver || *this == ver;
}

bool Version::operator > (const Version &ver) const
{
    if(m_major == ver.m_major) {
        if(m_minor == ver.m_minor) {
            return m_revision > ver.m_revision;
        }
        else return m_minor > ver.m_minor;
    }
    else return m_major > ver.m_major;
}

bool Version::operator >= (const Version &ver) const
{
    return *this > ver || *this == ver;
}

void Version::expose(DataFile::Node &node)
{
    node.var(m_major, "major");
    node.var(m_minor, "minor");
    node.var(m_revision, "revision");
}

void Version::setMajor(int major)
{
    m_major = major;
}

int Version::getMajor() const
{
    return m_major;
}

void Version::setMinor(int minor)
{
    m_minor = minor;
}

int Version::getMinor() const
{
    return m_minor;
}

void Version::setRevision(int revision)
{
    m_revision = revision;
}

int Version::getRevision() const
{
    return m_revision;
}

bool Version::fromString(const std::string &str)
{
    // TODO: clean

    std::string nums[3];
    std::string revString;

    size_t i{};

    while(i < str.size() && str[i] != '.') {
        nums[0] += str[i];
        ++i;
    }
    ++i;
    while(i < str.size() && str[i] != ' ') {
        nums[1] += str[i];
        ++i;
    }
    ++i;
    while(i < str.size() && str[i] != ' ') {
        revString += str[i];
        ++i;
    }
    ++i;
    while(i < str.size()) {
        nums[2] += str[i];
        ++i;
    }
    for(size_t i = 0; i < 3; ++i) {
        if(nums[i].empty())
            return false;

        for(size_t j = 0; j < nums[i].size(); ++j) {
            if(!isdigit(nums[i][j]) && !(j == 0 && nums[i][j] == '-'))
                return false;
        }
    }

    if(revString != "rev")
        return false;

    int major{}, minor{}, revision{};

    sscanf(nums[0].c_str(), "%d", &major);
    sscanf(nums[1].c_str(), "%d", &minor);
    sscanf(nums[2].c_str(), "%d", &revision);

    m_major = major;
    m_minor = minor;
    m_revision = revision;

    return true;
}

std::string Version::toString() const
{
    std::ostringstream oss;

    oss << m_major << '.' << m_minor << " rev " << m_revision;

    return oss.str();
}

} // namespace engine
