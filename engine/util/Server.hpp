#ifndef ENGINE_SERVER_HPP
#define ENGINE_SERVER_HPP

#include "Trace.hpp"
#include "Network.hpp"
#include "Packet.hpp"
#include "Version.hpp"

#include <string>
#include <unordered_map>

struct _ENetHost;
struct _ENetPeer;
struct _ENetEvent;

namespace engine
{

/*  Server manages only connected peers.
 *  It does not manage users data base
 *  with their logins and passwords.
 *  This is why there is no option for user
 *  banning but there is for kicking.
 *  Storing information about users other than
 *  connection state is not up to this class.
 *  User auth should be implemented using
 *  virtual methods.
*/

class Server : public Tracked <Server>
{
    static_assert(sizeof(intptr_t) == sizeof(void *),
                  "intptr_t does not have the same size as void *. "
                  "This is mandatory for storing peer ID.");

public:
    class AuthedUser
    {
    public:
        AuthedUser();

        void setPeer(_ENetPeer *peer);
        _ENetPeer *getPeer();

    private:
        _ENetPeer *m_peer;
    };

    Server();
    Server(const Server &) = delete;

    Server &operator = (const Server &) = delete;

    bool start(int port,
               Network::AuthMethod authMethod = Network::AuthMethod::None,
               const Version &appVersion = {},
               const std::string &serverPassword = {});
    bool update();
    void sendPacket(int ID, const Packet &packet);
    void broadcast(const Packet &packet);
    void disconnectUser(int ID, Network::DisconnectionReason reason = Network::DisconnectionReason::NoReason);
    void kickUser(int ID);
    void stop();

    virtual ~Server();

protected:
    virtual bool authUser(const std::string &login,
                          const std::string &password,
                          int &ID,
                          int &disconnectionReason);
    virtual bool authUser(int &ID, int &disconnectionReason);
    virtual void onUserAuth(int ID);
    virtual void onPacketReceive(int userID, const Packet &packet);
    virtual void onUserDisconnect(int ID);

private:
    void *IDToData(int ID);
    int dataToID(void *data);
    void sendEnginePacket(_ENetPeer &peer, const Packet &packet);
    void ENetConnect(_ENetEvent &event);
    void ENetReceive(_ENetEvent &event);
    void ENetDisconnect(_ENetEvent &event);

    static const size_t k_maxPeerCount;

    _ENetHost *m_host;
    Network::AuthMethod m_authMethod;
    Version m_appVersion;
    std::string m_serverPassword;
    std::unordered_map <int, AuthedUser> m_authedUsers;
};

} // namespace engine

#endif // ENGINE_SERVER_HPP
