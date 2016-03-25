#include "ResourceDef.hpp"

namespace engine
{
namespace app3D
{

void ResourceDef::setDevice(const std::weak_ptr <Device> &device)
{
    m_device = device;

    if(m_device.expired())
        throw Exception{"Device is nullptr."};

    reloadIrrObjects();
}

bool ResourceDef::deviceExpired() const
{
    return m_device.expired();
}

Device &ResourceDef::getDevice_slow()
{
    const auto &shared = m_device.lock();

    if(!shared)
        throw Exception{"Device is nullptr."};

    return *shared;
}

} // namespace app3D
} // namespace engine
