#ifndef APP_EFFECTS_POOL_HPP
#define APP_EFFECTS_POOL_HPP

#include "engine/util/Vec3.hpp"
#include "world/Effect.hpp"

namespace app
{

class EffectDef;

class EffectsPool
{
public:
    void update();

    bool add(const std::shared_ptr <EffectDef> &effectDef, const engine::FloatVec3 &pos);
    bool add(const std::shared_ptr <EffectDef> &effectDef, const engine::FloatVec3 &pos, const engine::FloatVec3 &rot);
    bool add_boxZone(const std::shared_ptr <EffectDef> &effectDef, const engine::FloatVec3 &pos, const engine::FloatVec3 &rot, const engine::FloatVec3 &boxDimension);
    bool add_sphereZone(const std::shared_ptr <EffectDef> &effectDef, const engine::FloatVec3 &pos, const engine::FloatVec3 &rot, float radius);

private:
    std::list <Effect> m_effects;
};

} // namespace app

#endif // APP_EFFECTS_POOL_HPP

