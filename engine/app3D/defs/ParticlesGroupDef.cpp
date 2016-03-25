#include "ParticlesGroupDef.hpp"

#include "../../util/DefDatabase.hpp"
#include "ParticlesGroupModelDef.hpp"
#include "ParticleSpriteDef.hpp"

namespace engine
{
namespace app3D
{

void ParticlesGroupDef::Emitter::ZoneSphere::expose(DataFile::Node &node)
{
    node.var(position, "position");
    node.var(radius, "radius");

    if(node.getActivityType() == DataFile::Activity::Type::Loading) {
        if(radius < 0.f)
            throw Exception{"Radius can't be negative."};
    }
}

ENUM_DEF(ParticlesGroupDef::Emitter::Type, Type,
    Normal,
    Random,
    Static,
    Spheric,
    Straight
);

ParticlesGroupDef::Emitter::Emitter()
    : m_type{Type::Normal},
      m_fullZone{true},
      m_flow{-1},
      m_tank{-1}
{
}

void ParticlesGroupDef::Emitter::expose(DataFile::Node &node)
{
    node.var(m_type, "type");
    node.var(m_direction, "direction", {});
    node.var(m_angle, "angle", {});
    node.var(m_zoneSphere, "zoneSphere");
    node.var(m_fullZone, "fullZone", true);
    node.var(m_flow, "flow");
    node.var(m_tank, "tank", -1);
    node.var(m_force, "force", {});
}

void ParticlesGroupDef::Emitter::multiplySizeBy(float factor)
{
    m_force *= factor;
    m_zoneSphere.position *= factor;
    m_zoneSphere.radius *= factor;
}

const ParticlesGroupDef::Emitter::Type &ParticlesGroupDef::Emitter::getType() const
{
    return m_type;
}

const FloatVec3 &ParticlesGroupDef::Emitter::getDirection() const
{
    return m_direction;
}

const FloatRange &ParticlesGroupDef::Emitter::getAngle() const
{
    return m_angle;
}

const ParticlesGroupDef::Emitter::ZoneSphere &ParticlesGroupDef::Emitter::getZoneSphere() const
{
    return m_zoneSphere;
}

int ParticlesGroupDef::Emitter::getFlow() const
{
    return m_flow;
}

int ParticlesGroupDef::Emitter::getTank() const
{
    return m_tank;
}

bool ParticlesGroupDef::Emitter::getFullZone() const
{
    return m_fullZone;
}

const FloatRange &ParticlesGroupDef::Emitter::getForce() const
{
    return m_force;
}

ParticlesGroupDef::ParticlesGroupDef()
    : m_maxParticlesCount{},
      m_friction{}
{
}

void ParticlesGroupDef::expose(DataFile::Node &node)
{
    base::expose(node);

    node.var(m_inheritScaledInfo, "inheritScaled", {});
    node.var(m_particlesGroupModelDef_defName, "particlesGroupModelDef");
    node.var(m_particleSpriteDef_defName, "particleSpriteDef");
    node.var(m_maxParticlesCount, "maxParticlesCount", 500);
    node.var(m_gravity, "gravity", {});
    node.var(m_friction, "friction", {});
    node.var(m_emitters, "emitters");

    if(node.getActivityType() == DataFile::Activity::Type::Loading) {
        if(!m_inheritScaledInfo.isLoaded) {
            if(m_maxParticlesCount < 0)
                throw Exception{"Max particles count can't be negative."};
        }
    }
}

void ParticlesGroupDef::onLoadedAllDefs(DefDatabase &defDatabase)
{
    if(m_inheritScaledInfo.isLoaded) {
        const auto &def = *defDatabase.getDef <ParticlesGroupDef> (m_inheritScaledInfo.def_defName);

        def.copyMembersScaled(*this, m_inheritScaledInfo.scale);
    }

    m_particlesGroupModelDef = defDatabase.getDef <ParticlesGroupModelDef> (m_particlesGroupModelDef_defName);
    m_particleSpriteDef = defDatabase.getDef <ParticleSpriteDef> (m_particleSpriteDef_defName);
}

void ParticlesGroupDef::dropIrrObjects()
{
}

void ParticlesGroupDef::reloadIrrObjects()
{
}

ParticlesGroupModelDef &ParticlesGroupDef::getParticlesGroupModelDef() const
{
    if(!m_particlesGroupModelDef)
        throw Exception{"Particles group model def is nullptr."};

    return *m_particlesGroupModelDef;
}

const std::shared_ptr <ParticlesGroupModelDef> &ParticlesGroupDef::getParticlesGroupModelDefPtr() const
{
    if(!m_particlesGroupModelDef)
        throw Exception{"Particles group model def is nullptr."};

    return m_particlesGroupModelDef;
}

ParticleSpriteDef &ParticlesGroupDef::getParticleSpriteDef() const
{
    if(!m_particleSpriteDef)
        throw Exception{"Particle sprite def is nullptr."};

    return *m_particleSpriteDef;
}

const std::shared_ptr <ParticleSpriteDef> &ParticlesGroupDef::getParticleSpriteDefPtr() const
{
    if(!m_particleSpriteDef)
        throw Exception{"Particle sprite def is nullptr."};

    return m_particleSpriteDef;
}

int ParticlesGroupDef::getMaxParticlesCount() const
{
    return m_maxParticlesCount;
}

const FloatVec3 &ParticlesGroupDef::getGravity() const
{
    return m_gravity;
}

float ParticlesGroupDef::getFriction() const
{
    return m_friction;
}

const std::vector <ParticlesGroupDef::Emitter> &ParticlesGroupDef::getEmitters() const
{
    return m_emitters;
}

void ParticlesGroupDef::InheritScaledInfo::expose(DataFile::Node &node)
{
    node.var(def_defName, "def");
    node.var(scale, "scale");

    if(node.getActivityType() == DataFile::Activity::Type::Loading) {
        if(scale < 0.f)
            throw Exception{"Scale can't be negative."};

        isLoaded = true;
    }
}

void ParticlesGroupDef::copyMembersScaled(ParticlesGroupDef &copyTo, float scale) const
{
    copyTo.m_maxParticlesCount = m_maxParticlesCount;
    copyTo.m_gravity = m_gravity;
    copyTo.m_friction = m_friction;
    copyTo.m_emitters = m_emitters;
    copyTo.m_gravity = m_gravity * scale;

    for(auto &elem : copyTo.m_emitters) {
        elem.multiplySizeBy(scale);
    }
}

} // namespace app3D
} // namespace engine
