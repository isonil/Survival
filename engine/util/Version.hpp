#ifndef ENGINE_VERSION_HPP
#define ENGINE_VERSION_HPP

#include "DataFile.hpp"

#include <string>

namespace engine
{

class Version : public DataFile::Saveable
{
public:
    Version();
    Version(int major, int minor, int revision);

    bool operator == (const Version &ver) const;
    bool operator != (const Version &ver) const;
    bool operator < (const Version &ver) const;
    bool operator <= (const Version &ver) const;
    bool operator > (const Version &ver) const;
    bool operator >= (const Version &ver) const;

    void expose(DataFile::Node &node) override;

    void setMajor(int major);
    int getMajor() const;
    void setMinor(int minor);
    int getMinor() const;
    void setRevision(int revision);
    int getRevision() const;

    bool fromString(const std::string &str);
    std::string toString() const;

private:
    int m_major, m_minor, m_revision;
};

} // namespace engine

#endif // ENGINE_VERSION_HPP
