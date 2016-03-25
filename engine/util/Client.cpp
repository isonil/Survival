#include "Client.hpp"

#include "../EngineStaticInfo.hpp"
#include "LogManager.hpp"

#include <enet/enet.h>

namespace engine
{

Client::Client()
    :   m_host{},
        m_peer{},
        m_connectionState{ConnectionState::Disconnected},
        m_authMethod{Network::AuthMethod::None},
        m_serverPort{}
{
}

bool Client::startConnecting(const std::string &addr,
                             int port,
                             const Version &appVersion,
                             const std::string &serverPassword,
                             const std::string &userLogin,
                             const std::string &userPassword)
{
    TRACK;

    if(!Network::ensureInit()) {
        E_ERROR("Could not initialize network.");
        return false;
    }

    if(!m_host) {
        m_host = enet_host_create(0, 1, 1, 0, 0);
        if(!m_host) {
            E_ERROR("Could not create network host.");
            return false;
        }
        m_host->maximumPacketSize = Network::k_maxPacketSize * 2;
        m_host->maximumWaitingData = Network::k_maxPacketSize * 100;
    }

    disconnect();

    ENetAddress address;
    address.port = port;
    enet_address_set_host(&address, addr.c_str());

    E_INFO("Connecting to server %s:%d.", addr.c_str(), port);

    m_peer = enet_host_connect(m_host, &address, 1, 0);
    if(!m_peer) {
        E_INFO("Could not create peer.");
        return false;
    }

    m_connectionState = ConnectionState::Connecting;
    m_appVersion = appVersion;
    m_serverPassword = serverPassword;
    m_userLogin = userLogin;
    m_userPassword = userPassword;
    m_serverAddress = addr;
    m_serverPort = port;

    return true;
}

void Client::disconnect()
{
    TRACK;

    if(!m_host) return;

    if(m_peer) {
        enet_peer_disconnect_now(m_peer, 0);
        m_peer = nullptr;
        E_INFO("Disconnected from server %s:%d.", m_serverAddress.c_str(), m_serverPort);
    }

    m_connectionState = ConnectionState::Disconnected;
    m_authMethod = Network::AuthMethod::None;
    m_appVersion = Version();
    m_serverPassword.clear();
    m_userLogin.clear();
    m_userPassword.clear();
    m_serverAddress.clear();
    m_serverPort = 0;

    while(!m_queuedAppPackets.empty()) {
        m_queuedAppPackets.pop();
    }
}

bool Client::update(std::string &outMessage, bool queueAppPackets)
{
    TRACK;

    outMessage.clear();

    if(!m_host) {
        outMessage = "Host was not created. Try connecting again.";
        return false;
    }

    if(m_connectionState == ConnectionState::Disconnected) {
        outMessage = "Disconnected from server.";
        return false;
    }

    E_RASSERT(m_peer, "Peer was not created but connection state is not disconnected and host is created.");

    if(!queueAppPackets) {
        while(!m_queuedAppPackets.empty()) {
            onPacketReceive(m_queuedAppPackets.front());
            m_queuedAppPackets.pop();
        }
    }

    ENetEvent event;

    while(true) {
        auto hostStatus = enet_host_service(m_host, &event, 0);

        if(hostStatus < 0) {
            E_ERROR("An error occured in enet_host_service. Network error. Disconnecting.");
            disconnect();
            outMessage = "Internal error.";
            return false;
        }

        if(!hostStatus) break;

        switch(event.type) {
        case ENET_EVENT_TYPE_NONE:
            break;

        case ENET_EVENT_TYPE_CONNECT:
            if(!ENetConnect(event, outMessage))
                return false;
            break;

        case ENET_EVENT_TYPE_RECEIVE:
            if(!ENetReceive(event, outMessage, queueAppPackets))
                return false;
            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            ENetDisconnect(event, outMessage);
            return false;

        default:
            break;
        }
    }

    return true;
}

void Client::sendPacket(const Packet &packet)
{
    TRACK;

    if(!m_host)
        return;

    if(m_connectionState != ConnectionState::Authed)
        return;

    E_RASSERT(m_peer, "Peer was not created but connection state is authed.");

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

    E_RASSERT(packetENet, "Could not create network app packet (enet_packet_create returned nullptr).");

    if(enet_peer_send(m_peer, 0, packetENet)) {
        E_WARNING("Engine error. Could not send network packet. Disconnecting.");
        disconnect();
        return;
    }

    enet_host_flush(m_host);
}

int Client::getPing()
{
    if(m_connectionState != ConnectionState::Disconnected) {
        E_RASSERT(m_peer, "Peer was not created but connection state is not disconnected.");
        return m_peer->lastRoundTripTime;
    }
    else return -1;
}

Client::ConnectionState Client::getConnectionState()
{
    return m_connectionState;
}

Client::~Client()
{
    TRACK;

    if(m_host)
        enet_host_destroy(m_host);
}

void Client::onPacketReceive(const Packet &packet)
{
}

void Client::sendEnginePacket(const Packet &packet)
{
    TRACK;

    if(!m_host)
        return;

    if(m_connectionState == ConnectionState::Disconnected)
        return;

    E_RASSERT(m_peer, "Peer was not created but connection state is not disconnected.");

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

    E_RASSERT(packetENet, "Could not create network engine packet (enet_packet_create returned nullptr).");

    if(enet_peer_send(m_peer, 0, packetENet)) {
        E_WARNING("Engine error. Could not send network engine packet. Disconnecting.");
        disconnect();
        return;
    }

    enet_host_flush(m_host);
}

bool Client::ENetConnect(ENetEvent &event, std::string &outMessage)
{
    TRACK;

    if(m_connectionState != ConnectionState::Connecting) {
        E_WARNING("Received ENet connection event while being already connected to server. Disconnecting.");
        outMessage = "Received connection event while being already connected to server. Probably server bug.";
        disconnect();
        return false;
    }

    E_INFO("Established connection with server %s:%d (%u:%u).",
           m_serverAddress.c_str(),
           m_serverPort,
           event.peer->address.host,
           event.peer->address.port);

    m_peer = event.peer;
    m_connectionState = ConnectionState::Connected;

    return true;
}

bool Client::ENetReceive(ENetEvent &event, std::string &outMessage, bool queueAppPackets)
{
    TRACK;

    if(event.packet->dataLength >= Network::k_packetHeaderSize &&
       event.packet->dataLength <= Network::k_maxPacketSize) {
        auto level = event.packet->data[0];

        auto *begin = event.packet->data + Network::k_packetHeaderSize;
        std::string packetData(begin, event.packet->data + event.packet->dataLength);

        Packet packet{static_cast <char> (event.packet->data[1]), packetData};

        if(level == static_cast <char> (Network::PacketLevel::App)) {
            if(m_connectionState == ConnectionState::Authed) {
                if(queueAppPackets)
                    m_queuedAppPackets.push(packet);
                else
                    onPacketReceive(packet);
            }
            else {
                // no, this packet should not be queued, it's more likely to be a server bug - old, outdated packet
                E_WARNING("Received app level packet while not being authed. Ignoring packet.");
            }
        }
        else if(level == static_cast <char> (Network::PacketLevel::Engine)) {
            if(packet.getType() == static_cast <char> (Network::EnginePacketType::ServerInfo)) {
                if(m_connectionState == ConnectionState::Authed) {
                    E_WARNING("Received server info packet while being already authed. Disconnecting.");

                    outMessage = "Received server info packet while being already authed. Probably server bug.";
                    disconnect();
                    return false;
                }

                std::string engineVersion, appVersion;
                int authMethod{-1};
                packet >> engineVersion >> appVersion >> authMethod;

                if(engineVersion != EngineStaticInfo::k_engineVersion.toString()) {
                    const auto &thisEngineVersionStr = EngineStaticInfo::k_engineVersion.toString();

                    E_INFO("Different server engine version (server: %s client: %s). Disconnecting.",
                           engineVersion.c_str(), thisEngineVersionStr.c_str());

                    outMessage = "Server version is different. Please update client.";
                    disconnect();
                    return false;
                }

                if(appVersion != m_appVersion.toString()) {
                    const auto &thisAppVersionStr = m_appVersion.toString();

                    E_INFO("Different server app version (server: %s client: %s). Disconnecting.",
                           appVersion.c_str(), thisAppVersionStr.c_str());

                    outMessage = "Server version is different. Please update client.";
                    disconnect();
                    return false;
                }

                if(authMethod == static_cast <int> (Network::AuthMethod::None)) {
                    m_authMethod = Network::AuthMethod::None;
                    Packet authPacket{static_cast <char> (Network::EnginePacketType::AuthRequest)};
                    sendEnginePacket(authPacket);
                }
                else if(authMethod == static_cast <int> (Network::AuthMethod::ServerPassword)) {
                    m_authMethod = Network::AuthMethod::ServerPassword;
                    Packet authPacket{static_cast <char> (Network::EnginePacketType::AuthRequest)};
                    authPacket << m_serverPassword;
                    sendEnginePacket(authPacket);
                }
                else if(authMethod == static_cast <int> (Network::AuthMethod::UserLoginAndPassword)) {
                    m_authMethod = Network::AuthMethod::UserLoginAndPassword;
                    Packet authPacket{static_cast <char> (Network::EnginePacketType::AuthRequest)};
                    authPacket << m_userLogin << m_userPassword;
                    sendEnginePacket(authPacket);
                }
                else if(authMethod == static_cast <int> (Network::AuthMethod::Both)) {
                    m_authMethod = Network::AuthMethod::Both;
                    Packet authPacket{static_cast <char> (Network::EnginePacketType::AuthRequest)};
                    authPacket << m_serverPassword << m_userLogin << m_userPassword;
                    sendEnginePacket(authPacket);
                }
                else {
                    E_WARNING("Received invalid auth method in server info packet (auth method: %d). Disconnecting.",
                              authMethod);

                    outMessage = "Received invalid auth method. Probably server bug.";
                    disconnect();
                    return false;
                }
            }
            else if(packet.getType() == static_cast <char> (Network::EnginePacketType::Authed)) {
                if(m_connectionState != ConnectionState::Authed) {
                    m_connectionState = ConnectionState::Authed;
                    E_INFO("Connected to server and authed successfully.");
                }
                else {
                    E_WARNING("Received authed packet while being already authed. Ignoring packet.");
                }
            }
            else {
                E_WARNING("Received engine level packet with unknown type (type as int: %d). Ignoring packet.",
                          static_cast <int> (packet.getType()));
            }
        }
        else {
            E_WARNING("Received packet with unknown level (level as int: %d). Ignoring packet.", (int)level);
        }
    }
    else {
        E_WARNING("Received packet with size %d (min: %d max: %d). Ignoring packet.",
                  static_cast <int> (event.packet->dataLength),
                  Network::k_packetHeaderSize,
                  Network::k_maxPacketSize);
    }
    enet_packet_destroy(event.packet);
    event.packet = nullptr;

    return true;
}

void Client::ENetDisconnect(ENetEvent &event, std::string &outMessage)
{
    TRACK;

    int reason = event.data;
    if(reason == static_cast <int> (Network::DisconnectionReason::NoReason)) {
        E_INFO("Received disconnect event. No reason. Disconnected from server.");
        outMessage = "Disconnected from server.";
    }
    else if(reason == static_cast <int> (Network::DisconnectionReason::IncorrectPass)) {
        E_INFO("Received disconnect event. Incorrect login or password. Disconnected from server.");

        if(m_authMethod == Network::AuthMethod::None) {
            E_WARNING("Received incorrect password packet but server said there was no auth method.");
            outMessage = "Incorrect login or password.";
        }
        else if(m_authMethod == Network::AuthMethod::UserLoginAndPassword ||
                m_authMethod == Network::AuthMethod::Both) {
            outMessage = "Incorrect login or password.";
        }
        else {
            outMessage = "Incorrect server password.";
        }
    }
    else if(reason == static_cast <int> (Network::DisconnectionReason::Kicked)) {
        E_INFO("Received disconnect event. Client kicked from server. Disconnected from server.");
        outMessage = "You were kicked from this server.";
    }
    else if(reason == static_cast <int> (Network::DisconnectionReason::IPBanned)) {
        E_INFO("Received disconnect event. Client IP banned from server. Disconnected from server.");
        outMessage = "Your IP is banned from this server.";
    }
    else if(reason == static_cast <int> (Network::DisconnectionReason::UserBanned)) {
        E_INFO("Received disconnect event. Client banned from server. Disconnected from server.");
        outMessage = "You are banned from this server.";
    }
    else if(reason == static_cast <int> (Network::DisconnectionReason::Timeout)) {
        E_INFO("Received disconnect event. Timeout reached. Disconnected from server.");
        outMessage = "Timeout reached. Disconnected from server.";
    }
    else if(reason == static_cast <int> (Network::DisconnectionReason::NetworkAbuse)) {
        E_INFO("Received disconnect event. Network abuse. Disconnected from server.");
        outMessage = "Network abuse. Probably client bug. Disconnected from server.";
    }
    else if(reason == static_cast <int> (Network::DisconnectionReason::SomeoneElseConnected)) {
        E_INFO("Received disconnect event. Someone else connected. Disconnected from server.");
        outMessage = "Someone else connected to this account.";
    }
    else if(reason == static_cast <int> (Network::DisconnectionReason::EngineError)) {
        E_INFO("Received disconnect event. Engine error. Disconnected from server.");
        outMessage = "Disconnected from server. Probably server bug.";
    }
    else if(reason == static_cast <int> (Network::DisconnectionReason::RepeatedLoginAbuse)) {
        E_INFO("Received disconnect event. Repeated login abuse. Disconnected from server.");
        outMessage = "Please wait a moment and try again.";
    }
    else if(reason == static_cast <int> (Network::DisconnectionReason::Other)) {
        E_INFO("Received disconnect event. Other reason. Disconnected from server.");
        outMessage = "Disconnected from server.";
    }
    else {
        E_WARNING("Received disconnect event with invalid reason (reason as int: %d). Disconnected from server.",
                  reason);
        outMessage = "Disconnected from server.";
    }
    m_peer = nullptr;
    disconnect();
}

} // namespace engine
