#include "SceneNode.hpp"

#include "../../util/Exception.hpp"
#include "../../util/LogManager.hpp"
#include "../Device.hpp"

namespace engine
{
namespace app3D
{

SceneNode::SceneNode(const std::weak_ptr <Device> &device)
    : m_device{device}
{
    TRACK;

    if(m_device.expired())
        throw Exception{"Device is nullptr."};
}

void SceneNode::update(const FloatVec3 &cameraPos, const AppTime &appTime)
{
}

bool SceneNode::deviceExpired() const
{
    return m_device.expired();
}

Device &SceneNode::getDevice_slow()
{
    const auto &shared = m_device.lock();

    if(!shared) {
        throw Exception{"Device is nullptr. If this exception was thrown in destructor, "
                              "then it should have been checked whether device has expired before using it."};
    }

    return *shared;
}

} // namespace app3D
} // namespace engine

