#ifndef APP_ANIMATION_FRAMES_SET_DEF_HPP
#define APP_ANIMATION_FRAMES_SET_DEF_HPP

#include "engine/util/Trace.hpp"
#include "engine/util/Def.hpp"
#include "engine/util/Range.hpp"

namespace app
{

class AnimationFramesSetDef : public engine::Def, public engine::Tracked <AnimationFramesSetDef>
{
public:
    void expose(engine::DataFile::Node &node) override;

    const engine::IntRange &getWalk() const;
    const engine::IntRange &getAttack() const;
    const engine::IntRange &getHarmed() const;
    const engine::IntRange &getDeath() const;
    const engine::IntRange &getOnSpawned() const;
    const engine::IntRange &getRun() const;
    const engine::IntRange &getIdle() const;

    const engine::IntRange &getIdle_FPP() const;
    const engine::IntRange &getUse0_FPP() const;
    const engine::IntRange &getUse1_FPP() const;
    const engine::IntRange &getUse2_FPP() const;
    const engine::IntRange &getReload_FPP() const;
    const engine::IntRange &getEquip_FPP() const;
    const engine::IntRange &getPutAway_FPP() const;

    const engine::IntRange &getUse_FPP_byIndex(int index) const;

    static const int k_maxUseAnimations;

private:
    using base = Def;

    // characters
    engine::IntRange m_walk;
    engine::IntRange m_attack;
    engine::IntRange m_harmed;
    engine::IntRange m_death;
    engine::IntRange m_onSpawned;
    engine::IntRange m_run;
    engine::IntRange m_idle;

    // FPP
    engine::IntRange m_idle_FPP;
    engine::IntRange m_use0_FPP;
    engine::IntRange m_use1_FPP;
    engine::IntRange m_use2_FPP;
    engine::IntRange m_reload_FPP;
    engine::IntRange m_equip_FPP;
    engine::IntRange m_putAway_FPP;
};

} // namespace app

#endif // APP_ANIMATION_FRAMES_SET_DEF_HPP
