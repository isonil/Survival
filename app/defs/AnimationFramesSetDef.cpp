#include "AnimationFramesSetDef.hpp"

namespace app
{

void AnimationFramesSetDef::expose(engine::DataFile::Node &node)
{
    TRACK;

    base::expose(node);

    node.var(m_walk, "walk", {});
    node.var(m_attack, "attack", {});
    node.var(m_harmed, "harmed", {});
    node.var(m_death, "death", {});
    node.var(m_onSpawned, "onSpawned", {});
    node.var(m_run, "run", {});
    node.var(m_idle, "idle", {});

    node.var(m_idle_FPP, "idle_FPP", {});
    node.var(m_use0_FPP, "use0_FPP", {});
    node.var(m_use1_FPP, "use1_FPP", {});
    node.var(m_use2_FPP, "use2_FPP", {});
    node.var(m_reload_FPP, "reload_FPP", {});
    node.var(m_equip_FPP, "equip_FPP", {});
    node.var(m_putAway_FPP, "putAway_FPP", {});
}

const engine::IntRange &AnimationFramesSetDef::getWalk() const
{
    return m_walk;
}

const engine::IntRange &AnimationFramesSetDef::getAttack() const
{
    return m_attack;
}

const engine::IntRange &AnimationFramesSetDef::getHarmed() const
{
    return m_harmed;
}

const engine::IntRange &AnimationFramesSetDef::getDeath() const
{
    return m_death;
}

const engine::IntRange &AnimationFramesSetDef::getOnSpawned() const
{
    return m_onSpawned;
}

const engine::IntRange &AnimationFramesSetDef::getRun() const
{
    return m_run;
}

const engine::IntRange &AnimationFramesSetDef::getIdle() const
{
    return m_idle;
}

const engine::IntRange &AnimationFramesSetDef::getIdle_FPP() const
{
    return m_idle_FPP;
}

const engine::IntRange &AnimationFramesSetDef::getUse0_FPP() const
{
    return m_use0_FPP;
}

const engine::IntRange &AnimationFramesSetDef::getUse1_FPP() const
{
    return m_use1_FPP;
}

const engine::IntRange &AnimationFramesSetDef::getUse2_FPP() const
{
    return m_use2_FPP;
}

const engine::IntRange &AnimationFramesSetDef::getReload_FPP() const
{
    return m_reload_FPP;
}

const engine::IntRange &AnimationFramesSetDef::getEquip_FPP() const
{
    return m_equip_FPP;
}

const engine::IntRange &AnimationFramesSetDef::getPutAway_FPP() const
{
    return m_putAway_FPP;
}

const engine::IntRange &AnimationFramesSetDef::getUse_FPP_byIndex(int index) const
{
    if(index == 0)
        return m_use0_FPP;
    else if(index == 1)
        return m_use1_FPP;
    else if(index == 2)
        return m_use2_FPP;

    throw engine::Exception{"Invalid use FPP animation index (" + std::to_string(index) + ")."};
}

const int AnimationFramesSetDef::k_maxUseAnimations{3};

} // namespace app
