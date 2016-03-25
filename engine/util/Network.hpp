#ifndef ENGINE_NETWORK_HPP
#define ENGINE_NETWORK_HPP

#include <cstddef>

namespace engine
{

class Network
{
public:
    enum class AuthMethod : int
    {
        None = 0,
        ServerPassword,
        UserLoginAndPassword,
        Both
    };

    enum class PacketLevel : char
    {
        Engine = '@',
        App = '#'
    };

    enum class EnginePacketType : char
    {
        ServerInfo = 'i',
        AuthRequest = 'r',
        Authed = 'a'
    };

    enum class DisconnectionReason : int
    {
        NoReason = 0,
        IncorrectPass = 1,
        Kicked = 2,
        IPBanned = 3,
        UserBanned = 4,
        Timeout = 5,
        NetworkAbuse = 6,
        SomeoneElseConnected = 7,
        EngineError = 8,
        RepeatedLoginAbuse = 9,
        Other = 10
    };

    static bool ensureInit();

    static const size_t k_packetHeaderSize;
    static const size_t k_maxPacketSize;

private:
    static bool m_isInitialized;
};

} // namespace engine

#endif // ENGINE_NETWORK_HPP
