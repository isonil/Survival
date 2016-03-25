#ifndef ENGINE_APP_3D_PARTICLES_GROUP_HPP
#define ENGINE_APP_3D_PARTICLES_GROUP_HPP

#include "../../util/Vec3.hpp"

#include <memory>

namespace SPK { class Group; }
namespace SPK { namespace IRR { class IRRSystem; } }

namespace engine
{
namespace app3D
{

class Device;
class ParticlesGroupDef;

class ParticlesGroup
{
public:
    ParticlesGroup(const std::weak_ptr <Device> &device, const std::shared_ptr <ParticlesGroupDef> &def);

    void setPosition(const FloatVec3 &pos);
    void setRotation(const FloatVec3 &rot);

    void setAllZonesToBox(const FloatVec3 &dimension);
    void setAllZonesToSphere(float radius);

    void stopAddingNewParticles();
    bool anyVisibleParticle() const;

    ~ParticlesGroup();

private:
    std::weak_ptr <Device> m_device;
    std::shared_ptr <ParticlesGroupDef> m_def;
    SPK::IRR::IRRSystem *m_SPKSystem;
    SPK::Group *m_SPKGroup;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_PARTICLES_GROUP_HPP
