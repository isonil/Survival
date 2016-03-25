#ifndef ENGINE_APP_3D_RESOURCE_DEF_HPP
#define ENGINE_APP_3D_RESOURCE_DEF_HPP

#include "../../util/Trace.hpp"
#include "../../util/Def.hpp"
#include "../IIrrlichtObjectsHolder.hpp"

#include <memory>

namespace engine
{
namespace app3D
{

class Device;

class ResourceDef : public Def, public IIrrlichtObjectsHolder, public Tracked <ResourceDef>
{
public:
    void setDevice(const std::weak_ptr <Device> &device);

protected:
    bool deviceExpired() const;
    Device &getDevice_slow();

private:
    std::weak_ptr <Device> m_device;
};

} // namesapce app3D
} // namespace engine

#endif // ENGINE_APP_3D_RESOURCE_DEF_HPP
