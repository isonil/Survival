#ifndef ENGINE_PACKET_HPP
#define ENGINE_PACKET_HPP

#include "Trace.hpp"
#include "DataFile.hpp"

#include <string>

namespace engine
{

class Packet : public DataFile::Saveable, public Tracked <Packet>
{
public:
    explicit Packet(char type = {}, const std::string &data = {});

    bool operator == (const Packet &packet) const;
    bool operator != (const Packet &packet) const;
    Packet &operator << (int val);
    Packet &operator << (float val);
    Packet &operator << (char val);
    Packet &operator << (const char *str);
    Packet &operator << (const std::string &str);
    Packet &operator >> (int &val);
    Packet &operator >> (float &val);
    Packet &operator >> (char &val);
    Packet &operator >> (std::string &str);

    void expose(DataFile::Node &node) override;

    char getType() const;
    std::string getData() const;

    void setType(char type);
    void setData(const std::string &data);

private:
    char m_type;
    std::string m_data;
    size_t m_readPos;
};

} // namespace engine

#endif // ENGINE_PACKET_HPP
