#include "ParticlesGroup.hpp"

#include "../defs/ParticlesGroupDef.hpp"
#include "../defs/ParticlesGroupModelDef.hpp"
#include "../defs/ParticleSpriteDef.hpp"
#include "../Device.hpp"
#include "../IrrlichtConversions.hpp"

#undef emit // qt keyword
#include <SPK.h>
#include <SPK_IRR.h>

namespace engine
{
namespace app3D
{

ParticlesGroup::ParticlesGroup(const std::weak_ptr <Device> &device, const std::shared_ptr <ParticlesGroupDef> &def)
    : m_device{device},
      m_def{def},
      m_SPKSystem{},
      m_SPKGroup{}
{
    if(!m_def)
        throw Exception{"Particles group def is nullptr."};

    const auto &deviceShared = m_device.lock();

    if(!deviceShared)
        throw Exception{"Device is nullptr."};

    auto &particlesGroupModelDef = m_def->getParticlesGroupModelDef();

    m_SPKGroup = SPK::Group::create(&particlesGroupModelDef.getSPKModel(), m_def->getMaxParticlesCount());

    if(!m_SPKGroup)
        throw Exception{"Could not create SPARK group."};

    const auto &gravity = m_def->getGravity();

    m_SPKGroup->setGravity({gravity.x, gravity.y, gravity.z});
    m_SPKGroup->setFriction(m_def->getFriction());
    m_SPKGroup->setRenderer(&m_def->getParticleSpriteDef().getSPKIrrQuadRenderer());
    m_SPKGroup->enableAABBComputing(true);

    for(const auto &elem : m_def->getEmitters()) {
        SPK::Emitter *emitter{};

        if(elem.getType() == ParticlesGroupDef::Emitter::Type::Normal)
            emitter = SPK::NormalEmitter::create();
        else if(elem.getType() == ParticlesGroupDef::Emitter::Type::Random)
            emitter = SPK::RandomEmitter::create();
        else if(elem.getType() == ParticlesGroupDef::Emitter::Type::Static)
            emitter = SPK::StaticEmitter::create();
        else if(elem.getType() == ParticlesGroupDef::Emitter::Type::Spheric) {
            const auto &dir = elem.getDirection();
            const auto &angle = elem.getAngle();

            emitter = SPK::SphericEmitter::create(SPK::Vector3D{dir.x, dir.y, dir.z}, angle.from, angle.to);
        }
        else if(elem.getType() == ParticlesGroupDef::Emitter::Type::Straight) {
            const auto &dir = elem.getDirection();

            emitter = SPK::StraightEmitter::create(SPK::Vector3D{dir.x, dir.y, dir.z});
        }

        if(!emitter)
            throw Exception{"Could not create SPARK emitter."};

        const auto &zoneSphere = elem.getZoneSphere();
        const auto &force = elem.getForce();

        emitter->setZone(SPK::Sphere::create(SPK::Vector3D{zoneSphere.position.x, zoneSphere.position.y, zoneSphere.position.z}, zoneSphere.radius), elem.getFullZone());
        emitter->setFlow(elem.getFlow());
        emitter->setTank(elem.getTank());
        emitter->setForce(force.from, force.to);

        m_SPKGroup->addEmitter(emitter);
    }

    auto &irrSceneManager = *deviceShared->getIrrDevice().getSceneManager();

    m_SPKSystem = SPK::IRR::IRRSystem::create(irrSceneManager.getRootSceneNode(), &irrSceneManager);

    E_DASSERT(m_SPKSystem, "SPARK system is nullptr.");

    m_SPKSystem->addGroup(m_SPKGroup);
    m_SPKSystem->enableAABBComputing(true);
}

void ParticlesGroup::setPosition(const FloatVec3 &pos)
{
    E_DASSERT(m_SPKSystem, "SPARK system is nullptr.");

    m_SPKSystem->setPosition(IrrlichtConversions::toVector(pos));
}

void ParticlesGroup::setRotation(const FloatVec3 &rot)
{
    E_DASSERT(m_SPKSystem, "SPARK system is nullptr.");

    m_SPKSystem->setRotation(IrrlichtConversions::toVector(rot));
}

void ParticlesGroup::setAllZonesToBox(const FloatVec3 &dimension)
{
    E_DASSERT(m_SPKGroup, "SPARK group is nullptr.");

    for(auto &elem : m_SPKGroup->getEmitters()) {
        E_DASSERT(elem, "SPARK emitter is nullptr.");
        elem->setZone(SPK::AABox::create({}, {dimension.x, dimension.y, dimension.z}));
    }
}

void ParticlesGroup::setAllZonesToSphere(float radius)
{
    E_DASSERT(m_SPKGroup, "SPARK group is nullptr.");

    for(auto &elem : m_SPKGroup->getEmitters()) {
        E_DASSERT(elem, "SPARK emitter is nullptr.");
        elem->setZone(SPK::Sphere::create({}, radius));
    }
}

void ParticlesGroup::stopAddingNewParticles()
{
    E_DASSERT(m_SPKGroup, "SPARK group is nullptr.");

    for(auto &elem : m_SPKGroup->getEmitters()) {
        E_DASSERT(elem, "SPARK emitter is nullptr.");
        elem->setActive(false);
    }
}

bool ParticlesGroup::anyVisibleParticle() const
{
    E_DASSERT(m_SPKGroup, "SPARK group is nullptr.");

    return static_cast <bool> (m_SPKGroup->getNbParticles());
}

ParticlesGroup::~ParticlesGroup()
{
    const auto &deviceShared = m_device.lock();

    if(deviceShared) {
        m_SPKSystem->removeGroup(m_SPKGroup);
        m_SPKSystem->remove();
    }
}

} // namespace app3D
} // namespace engine
