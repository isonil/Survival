#include "Packet.hpp"

#include <cstring>

namespace engine
{

Packet::Packet(char type, const std::string &data)
    : m_type{type},
      m_data{data},
      m_readPos{}
{
}

bool Packet::operator == (const Packet &packet) const
{
    return m_type == packet.m_type && m_data == packet.m_data;
}

bool Packet::operator != (const Packet &packet) const
{
    return !(*this == packet);
}

Packet &Packet::operator << (int val)
{
    TRACK;

    m_data.append(reinterpret_cast <char *> (&val), sizeof(val));
    return *this;
}

Packet &Packet::operator << (float val)
{
    TRACK;

    m_data.append(reinterpret_cast <char *> (&val), sizeof(val));
    return *this;
}

Packet &Packet::operator << (char val)
{
    TRACK;

    m_data += val;
    return *this;
}

Packet &Packet::operator << (const char *str)
{
    TRACK;

    auto len = strlen(str);
    *this << static_cast <int> (len);
    m_data += str;

    return *this;
}

Packet &Packet::operator << (const std::string &str)
{
    TRACK;

    *this << static_cast <int> (str.size());
    m_data += str;

    return *this;
}

Packet &Packet::operator >> (int &val)
{
    TRACK;

    if(m_data.size() >= sizeof(val) &&
       m_readPos <= m_data.size() - sizeof(val)) {
        val = *(reinterpret_cast <const int *> (m_data.c_str() + m_readPos));
        m_readPos += sizeof(val);
    }
    else val = 0;

    return *this;
}

Packet &Packet::operator >> (float &val)
{
    TRACK;

    if(m_data.size() >= sizeof(val) &&
       m_readPos <= m_data.size()-sizeof(val)) {
        val = *(reinterpret_cast <const float *> (m_data.c_str() + m_readPos));
        m_readPos += sizeof(val);
    }
    else val = 0.f;

    return *this;
}

Packet &Packet::operator >> (char &val)
{
    TRACK;

    if(!m_data.empty() && m_readPos < m_data.size()) {
        val = m_data[m_readPos];
        ++m_readPos;
    }
    else val = '\0';

    return *this;
}

Packet &Packet::operator >> (std::string &str)
{
    TRACK;

    int strLen{};
    *this >> strLen;

    str = "";
    if(strLen > 0) {
        if(static_cast <int> (m_data.size()) >= strLen &&
           m_readPos <= m_data.size() - (size_t)strLen) {
            str.append(m_data.c_str() + m_readPos, strLen);
            m_readPos += strLen;
        }
    }

    return *this;
}

void Packet::expose(DataFile::Node &node)
{
    node.var(m_type, "type");
    node.var(m_data, "data");
    node.var(m_readPos, "readPos");
}

char Packet::getType() const
{
    return m_type;
}

std::string Packet::getData() const
{
    return m_data;
}

void Packet::setType(char type)
{
    m_type = type;
}

void Packet::setData(const std::string &data)
{
    m_data = data;
    m_readPos = 0;
}

} // namespace engine
