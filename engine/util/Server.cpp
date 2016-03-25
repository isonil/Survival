#include "Server.hpp"

#include "../EngineStaticInfo.hpp"
#include "LogManager.hpp"

#include <enet/enet.h>

namespace engine
{

Server::AuthedUser::AuthedUser()
    : m_peer{}
{
}

void Server::AuthedUser::setPeer(_ENetPeer *peer)
{
    m_peer = peer;
}

_ENetPeer *Server::AuthedUser::getPeer()
{
    return m_peer;
}

Server::Server()
    :   m_host{},
        m_authMethod{Network::AuthMethod::None}
{
}

bool Server::start(int port,
                   Network::AuthMethod authMethod,
                   const Version &appVersion,
                   const std::string &serverPassword)
{
    TRACK;

    if(!Network::ensureInit()) {
        E_ERROR("Could not inialize network.");
        return false;
    }

    if(m_host) stop();

    E_INFO("Starting server (port %d).", port);

    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;

    m_host = enet_host_create(&address, k_maxPeerCount, 1, 0, 0);
    if(!m_host) {
        E_INFO("Could not create host.");
        return false;
    }

    m_host->maximumPacketSize = Network::k_maxPacketSize * 2;
    m_host->maximumWaitingData = Network::k_maxPacketSize * 100;

    m_authMethod = authMethod;
    m_appVersion = appVersion;
    m_serverPassword = serverPassword;

    return true;
}

bool Server::update()
{
    TRACK;

    if(!m_host) {
        E_ERROR("Cannot update server, host is not created.");
        return false;
    }

    ENetEvent event;

    while(true) {
        auto hostStatus = enet_host_service(m_host, &event, 0);

        if(hostStatus < 0) {
            E_ERROR("An error occured in enet_host_service. Network error. Stopping server.");
            stop();
            return false;
        }

        if(!hostStatus)
            break;

        switch(event.type) {
        case ENET_EVENT_TYPE_NONE:
            break;

        case ENET_EVENT_TYPE_CONNECT:
            ENetConnect(event);
            break;

        case ENET_EVENT_TYPE_RECEIVE:
            ENetReceive(event);
            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            ENetDisconnect(event);
            break;

        default:
            break;
        }
    }

    return true;
}

void Server::sendPacket(int ID, const Packet &packet)
{
    TRACK;

    if(!m_host)
        return;

    auto user = m_authedUsers.find(ID);

    if(user == m_authedUsers.end()) {
        E_WARNING("Tried to send packet to non-existing user with id %d.", ID);
        return;
    }

    if(!user->second.getPeer()) {
        E_WARNING("Engine error. Tried to send packet to nullptr peer. User is authed but his peer is nullptr. Disconnecting user.");
        disconnectUser(ID, Network::DisconnectionReason::EngineError);
        return;
    }

    const auto &packetData = packet.getData();

    size_t size{Network::k_packetHeaderSize + packetData.size()};
    if(size > Network::k_maxPacketSize) {
        E_WARNING("Tried to send packet with size greater than k_maxPacketSize. Ignoring packet.");
        return;
    }

    std::string packetPacked(size, '\0');
    packetPacked[0] = static_cast <char> (Network::PacketLevel::App);
    packetPacked[1] = packet.getType();

    std::copy(packetData.begin(), packetData.end(), packetPacked.begin() + Network::k_packetHeaderSize);

    auto *packetENet = enet_packet_create(packetPacked.data(), size, ENET_PACKET_FLAG_RELIABLE);

    if(!packetENet) {
        E_WARNING("Could not create network app packet (enet_packet_create returned nullptr). Ignoring packet.");
        return;
    }

    if(enet_peer_send(user->second.getPeer(), 0, packetENet)) {
        E_WARNING("Engine error. Could not send network app packet to user. Disconnecting user.");
        disconnectUser(ID, Network::DisconnectionReason::EngineError);
        return;
    }

    enet_host_flush(m_host);
}

void Server::broadcast(const Packet &packet)
{
    TRACK;

    if(!m_host)
        return;

    // sendPacket can fail and then disconnect user
    for(auto it = m_authedUsers.begin(); it != m_authedUsers.end();) {
        auto prevIt = it;
        ++it;
        sendPacket(prevIt->first, packet);
    }
}

void Server::disconnectUser(int ID, Network::DisconnectionReason reason)
{
    TRACK;

    if(!m_host)
        return;

    auto user = m_authedUsers.find(ID);

    if(user == m_authedUsers.end())
        return;

    if(user->second.getPeer()) {
        user->second.getPeer()->data = IDToData(-1);
        enet_peer_disconnect_now(user->second.getPeer(), static_cast <int> (reason));
        user->second.setPeer(nullptr);
    }

    onUserDisconnect(ID);
    m_authedUsers.erase(user);
}

void Server::kickUser(int ID)
{
    TRACK;

    if(!m_host)
        return;

    auto user = m_authedUsers.find(ID);

    if(user == m_authedUsers.end())
        return;

    if(user->second.getPeer()) {
       enet_peer_disconnect(user->second.getPeer(), static_cast <int> (Network::DisconnectionReason::Kicked));
       // ENet disconnection event will occur
    }
    else {
        E_WARNING("Engine error. User peer is nullptr but he is authed (discovered while kicking user). Disconnecting user.");
        disconnectUser(ID, Network::DisconnectionReason::EngineError);
    }
}

void Server::stop()
{
    TRACK;

    if(!m_host)
        return;

    for(auto it = m_authedUsers.begin(); it != m_authedUsers.end();) {
        auto prevIt = it;
        ++it;
        disconnectUser(prevIt->first);
    }

    enet_host_destroy(m_host);
    m_authedUsers.clear();
    m_host = nullptr;
}

Server::~Server()
{
    TRACK;

    if(m_host)
        enet_host_destroy(m_host);
}

bool Server::authUser(const std::string &login,
                      const std::string &password,
                      int &ID,
                      int &disconnectionReason)
{
    return false;
}

bool Server::authUser(int &ID, int &disconnectionReason)
{
    return false;
}

void Server::onUserAuth(int ID)
{
}

void Server::onPacketReceive(int userID, const Packet &packet)
{
}

void Server::onUserDisconnect(int ID)
{
}

void *Server::IDToData(int ID)
{
    return reinterpret_cast <void *> (static_cast <intptr_t> (ID));
}

int Server::dataToID(void *data)
{
    return static_cast <int> (reinterpret_cast <intptr_t> (data));
}

void Server::sendEnginePacket(ENetPeer &peer, const Packet &packet)
{
    TRACK;

    if(!m_host)
        return;

    const auto &packetData = packet.getData();

    size_t size{Network::k_packetHeaderSize + packetData.size()};
    if(size > Network::k_maxPacketSize) {
        E_WARNING("Tried to send engine packet with size greater than k_maxPacketSize. Ignoring packet.");
        return;
    }

    std::string packetPacked(size, '\0');
    packetPacked[0] = static_cast <char> (Network::PacketLevel::Engine);
    packetPacked[1] = packet.getType();

    std::copy(packetData.begin(), packetData.end(), packetPacked.begin() + Network::k_packetHeaderSize);

    auto *packetENet = enet_packet_create(packetPacked.data(), size, ENET_PACKET_FLAG_RELIABLE);

    if(!packetENet) {
        E_WARNING("Could not create network engine packet (enet_packet_create returned nullptr). Ignoring packet.");
        return;
    }

    if(enet_peer_send(&peer, 0, packetENet)) {
        E_WARNING("Engine error. Could not send network engine packet to user. Disconnecting user.");
        enet_peer_disconnect(&peer, static_cast <int> (Network::DisconnectionReason::EngineError));
        return;
    }

    enet_host_flush(m_host);
}

void Server::ENetConnect(ENetEvent &event)
{
    TRACK;

    E_DASSERT(event.peer, "Peer is nullptr.");

    event.peer->data = IDToData(-1);

    Packet serverInfoPacket{static_cast <char> (Network::EnginePacketType::ServerInfo)};
    serverInfoPacket << EngineStaticInfo::k_engineVersion.toString()
                     << m_appVersion.toString()
                     << static_cast <int> (m_authMethod);

    sendEnginePacket(*event.peer, serverInfoPacket);
}

void Server::ENetReceive(ENetEvent &event)
{
    TRACK;

    auto userID = dataToID(event.peer->data);

    if(event.packet->dataLength < Network::k_packetHeaderSize &&
       event.packet->dataLength > Network::k_maxPacketSize) {
        if(userID >= 0)
            disconnectUser(userID, Network::DisconnectionReason::NetworkAbuse);
        else
            enet_peer_disconnect_now(event.peer, static_cast <int> (Network::DisconnectionReason::NetworkAbuse));
    }
    else {
        auto level = event.packet->data[0];

        auto *begin = event.packet->data + Network::k_packetHeaderSize;
        std::string packetData(begin, event.packet->data + event.packet->dataLength);

        Packet packet{static_cast <char> (event.packet->data[1]), packetData};

        if(level == static_cast <char> (Network::PacketLevel::App)) {
            if(userID >= 0)
                onPacketReceive(userID, packet);
            else // user sent App level packet, but he was not authed
                enet_peer_disconnect_now(event.peer, static_cast <int> (Network::DisconnectionReason::NetworkAbuse));
        }
        else if(level == static_cast <char> (Network::PacketLevel::Engine)) {
            if(packet.getType() == static_cast <char> (Network::EnginePacketType::AuthRequest)) {
                if(userID < 0) {
                    bool authed{};
                    int ID{-1};
                    auto disconnectionReason = static_cast <int> (Network::DisconnectionReason::NoReason);

                    if(m_authMethod == Network::AuthMethod::None) {
                        if(authUser(ID, disconnectionReason))
                            authed = true;
                    }
                    else if(m_authMethod == Network::AuthMethod::ServerPassword) {
                        std::string serverPassword;
                        packet >> serverPassword;

                        if(serverPassword != m_serverPassword)
                            disconnectionReason = static_cast <int> (Network::DisconnectionReason::IncorrectPass);
                        else if(authUser(ID, disconnectionReason))
                            authed = true;
                    }
                    else if(m_authMethod == Network::AuthMethod::UserLoginAndPassword) {
                        std::string userLogin, userPassword;
                        packet >> userLogin >> userPassword;

                        if(authUser(userLogin, userPassword, ID, disconnectionReason))
                            authed = true;
                    }
                    else if(m_authMethod == Network::AuthMethod::Both) {
                        std::string serverPassword, userLogin, userPassword;
                        packet >> serverPassword >> userLogin >> userPassword;

                        if(serverPassword != m_serverPassword)
                            disconnectionReason = static_cast <int> (Network::DisconnectionReason::IncorrectPass);
                        else if(authUser(userLogin, userPassword, ID, disconnectionReason))
                            authed = true;
                    }

                    if(!authed || ID < 0)
                        enet_peer_disconnect(event.peer, disconnectionReason);
                    else {
                        auto user = m_authedUsers.find(ID);

                        if(user != m_authedUsers.end())
                            disconnectUser(ID, Network::DisconnectionReason::SomeoneElseConnected);

                        event.peer->data = IDToData(ID);
                        m_authedUsers[ID].setPeer(event.peer);

                        Packet packet{static_cast <char> (Network::EnginePacketType::Authed)};
                        sendEnginePacket(*event.peer, packet);

                        onUserAuth(ID);
                    }
                }
                else // authed user tried to auth
                    disconnectUser(userID, Network::DisconnectionReason::NetworkAbuse);
            }
        }
        else { // user sent packet with unknown level
            if(userID >= 0)
                disconnectUser(userID, Network::DisconnectionReason::NetworkAbuse);
            else
                enet_peer_disconnect_now(event.peer, static_cast <int> (Network::DisconnectionReason::NetworkAbuse));
        }
    }

    enet_packet_destroy(event.packet);
}

void Server::ENetDisconnect(ENetEvent &event)
{
    TRACK;

    auto ID = dataToID(event.peer->data);

    if(ID >= 0)
        disconnectUser(ID);

    event.peer->data = IDToData(-1);
}

const size_t Server::k_maxPeerCount{64};

} // namespace engine
