#ifndef ENGINE_CLIENT_HPP
#define ENGINE_CLIENT_HPP

#include "Trace.hpp"
#include "Network.hpp"
#include "Packet.hpp"
#include "Version.hpp"

#include <string>
#include <queue>

struct _ENetHost;
struct _ENetPeer;
struct _ENetEvent;

namespace engine
{

class Client : public Tracked <Client>
{
    static_assert(sizeof(intptr_t) == sizeof(void *),
                  "intptr_t does not have the same size as void *. "
                  "This is mandatory for storing peer ID.");

public:
    enum class ConnectionState
    {
        Disconnected,
        Connecting,
        Connected,
        Authed
    };

    Client();
    Client(const Client &) = delete;

    Client &operator = (const Client &) = delete;

    bool startConnecting(const std::string &addr,
                         int port,
                         const Version &appVersion = {},
                         const std::string &serverPassword = {},
                         const std::string &userLogin = {},
                         const std::string &userPassword = {});
    void disconnect();
    /* If update with queueAppPackets == true is called,
     * app level packets will be queued until next
     * update call with queueAppPackets == false.
     * Usually it's used when peer is still connecting,
     * and update is used only until peer is connected
     * and authed (so onPacketReceive is not called until queueAppPackets == true).
     * update() returns false if there was a non-fatal error (outMessage contains an error).
     * connect() is then required to be called again in order to connect to the server.
     */
    bool update(std::string &outMessage, bool queueAppPackets = false);
    void sendPacket(const Packet &packet);

    int getPing();
    ConnectionState getConnectionState();

    virtual ~Client();

protected:
    virtual void onPacketReceive(const Packet &packet);

private:
    void sendEnginePacket(const Packet &packet);
    bool ENetConnect(_ENetEvent &event, std::string &outMessage);
    bool ENetReceive(_ENetEvent &event, std::string &outMessage, bool queueAppPackets);
    void ENetDisconnect(_ENetEvent &event, std::string &outMessage);

    _ENetHost *m_host;
    _ENetPeer *m_peer;
    ConnectionState m_connectionState;
    Network::AuthMethod m_authMethod;
    Version m_appVersion;
    std::string m_serverPassword;
    std::string m_userLogin;
    std::string m_userPassword;
    std::string m_serverAddress;
    int m_serverPort;
    std::queue <Packet> m_queuedAppPackets;
};

} // namespace engine

#endif // ENGINE_CLIENT_HPP
