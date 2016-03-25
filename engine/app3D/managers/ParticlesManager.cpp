#include "ParticlesManager.hpp"

#include "../defs/ParticlesGroupDef.hpp"
#include "../particles/ParticlesGroup.hpp"
#include "../Device.hpp"

#undef emit // qt keyword
#include <SPK.h>
#include <SPK_IRR.h>

namespace engine
{
namespace app3D
{

ParticlesManager::ParticlesManager(Device &device)
    : m_device{device}
{
    SPK::System::setClampStep(true, k_clampStep);
    SPK::System::useAdaptiveStep(k_adaptiveStep.from, k_adaptiveStep.to);
}

void ParticlesManager::update()
{
    for(size_t i = 0; i < m_particlesGroups.size();) {
        if(m_particlesGroups[i].unique()) {
            std::swap(m_particlesGroups[i], m_particlesGroups.back());
            m_particlesGroups.pop_back();
        }
        else
            ++i;
    }
}

std::shared_ptr <ParticlesGroup> ParticlesManager::addParticlesGroup(const std::shared_ptr <ParticlesGroupDef> &def)
{
    const auto &ptr = std::make_shared <ParticlesGroup> (m_device.getPtr(), def);
    m_particlesGroups.push_back(ptr);
    return ptr;
}

// TODO: remove
ParticlesManager::~ParticlesManager()
{
}

const float ParticlesManager::k_clampStep{0.1f};
const FloatRange ParticlesManager::k_adaptiveStep{0.001f, 0.01f};

} // namespace app3D
} // namespace engine
