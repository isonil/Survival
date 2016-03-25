#include "Network.hpp"

#include "LogManager.hpp"
#include "Trace.hpp"

#include <enet/enet.h>

namespace engine
{

bool Network::ensureInit()
{
    TRACK;

    if(!m_isInitialized) {
        E_INFO("Initializing ENet %d.%d.%d.",
               ENET_VERSION_MAJOR,
               ENET_VERSION_MINOR,
               ENET_VERSION_PATCH);

        if(enet_initialize()) {
            E_ERROR("Could not initialize ENet (network library).");
            return false;
        }

        atexit(enet_deinitialize);
        E_INFO("ENet initialized.");
        m_isInitialized = true;
    }

    return true;
}

const size_t Network::k_packetHeaderSize{2}; // packet level and type
const size_t Network::k_maxPacketSize{2048};
bool Network::m_isInitialized{};

} // namespace engine
