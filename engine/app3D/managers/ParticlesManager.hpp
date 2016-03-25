#ifndef ENGINE_APP_3D_PARTICLES_MANAGER_HPP
#define ENGINE_APP_3D_PARTICLES_MANAGER_HPP

#include "../../util/Range.hpp"

namespace SPK { class System; class Group; }

namespace engine
{
namespace app3D
{

class Device;
class ParticlesGroup;
class ParticlesGroupDef;

class ParticlesManager
{
public:
    ParticlesManager(Device &device);

    void update();

    std::shared_ptr <ParticlesGroup> addParticlesGroup(const std::shared_ptr <ParticlesGroupDef> &def);

    ~ParticlesManager();

private:
    static const float k_clampStep;
    static const FloatRange k_adaptiveStep;

    Device &m_device;

    std::vector <std::shared_ptr <ParticlesGroup>> m_particlesGroups;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_PARTICLES_MANAGER_HPP

