#ifndef APP_ENTITY_DEF_HPP
#define APP_ENTITY_DEF_HPP

#include "engine/util/Trace.hpp"
#include "engine/util/Def.hpp"

namespace engine { namespace app3D { class SoundDef; } }

namespace app
{

class EffectDef;
class CachedCollisionShapeDef;

class EntityDef : public engine::Def, public engine::Tracked <EntityDef>
{
public:
    EntityDef();

    void expose(engine::DataFile::Node &node) override;

    CachedCollisionShapeDef &getCachedCollisionShapeDef() const;
    const std::shared_ptr <CachedCollisionShapeDef> &getCachedCollisionShapeDefPtr() const;
    bool hasStepSound() const;
    const std::shared_ptr <engine::app3D::SoundDef> &getStepSoundDefPtr() const;
    EffectDef &getOnHitEffectDef() const;
    const std::shared_ptr <EffectDef> &getOnHitEffectDefPtr() const;
    float getMass() const;
    bool hasMass() const;

private:
    using base = Def;

    float m_mass;
    std::string m_cachedCollisionShapeDef_defName;
    std::shared_ptr <CachedCollisionShapeDef> m_cachedCollisionShapeDef;
    std::string m_onHitEffectDef_defName;
    std::shared_ptr <EffectDef> m_onHitEffectDef;
    std::string m_stepSoundDef_defName;
    std::shared_ptr <engine::app3D::SoundDef> m_stepSoundDef;
};

} // namespace app

#endif // APP_ENTITY_DEF_HPP
