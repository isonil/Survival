#include "CharacterStatsChange.hpp"

#include "engine/util/StringUtility.hpp"
#include "engine/GUI/GUIManager.hpp"
#include "engine/GUI/IGUIRenderer.hpp"
#include "engine/app3D/Device.hpp"
#include "../../Global.hpp"
#include "../../Core.hpp"

namespace app
{

CharacterStatsChange::CharacterStatsChange()
    : m_moveSpeedMultiplier{1.f},
      m_fallDamageMultiplier{1.f},
      m_meleeDamageMultiplier{1.f},
      m_rangedDamageMultiplier{1.f},
      m_meleeDamageWhenDefendingMultiplier{1.f},
      m_rangedDamageWhenDefendingMultiplier{1.f},
      m_maxHPDiff{},
      m_recoilMultiplier{1.f},
      m_minProjectilesSpreadAngleMultiplier{1.f},
      m_jumpVelocityMultiplier{1.f},
      m_HPRegenerationPer5SecDiff{}
{
}

bool CharacterStatsChange::operator == (const CharacterStatsChange &rhs) const
{
    return m_moveSpeedMultiplier == rhs.m_moveSpeedMultiplier &&
           m_fallDamageMultiplier == rhs.m_fallDamageMultiplier &&
           m_meleeDamageMultiplier == rhs.m_meleeDamageMultiplier &&
           m_rangedDamageMultiplier == rhs.m_rangedDamageMultiplier &&
           m_meleeDamageWhenDefendingMultiplier == rhs.m_meleeDamageWhenDefendingMultiplier &&
           m_rangedDamageWhenDefendingMultiplier == rhs.m_rangedDamageWhenDefendingMultiplier &&
           m_maxHPDiff == rhs.m_maxHPDiff &&
           m_recoilMultiplier == rhs.m_recoilMultiplier &&
           m_minProjectilesSpreadAngleMultiplier == rhs.m_minProjectilesSpreadAngleMultiplier &&
           m_jumpVelocityMultiplier == rhs.m_jumpVelocityMultiplier &&
           m_HPRegenerationPer5SecDiff == rhs.m_HPRegenerationPer5SecDiff;
}

bool CharacterStatsChange::operator != (const CharacterStatsChange &rhs) const
{
    return !(*this == rhs);
}

void CharacterStatsChange::expose(engine::DataFile::Node &node)
{
    node.var(m_moveSpeedMultiplier, "moveSpeedMultiplier", 1.f);
    node.var(m_fallDamageMultiplier, "fallDamageMultiplier", 1.f);
    node.var(m_meleeDamageMultiplier, "meleeDamageMultiplier", 1.f);
    node.var(m_rangedDamageMultiplier, "rangedDamageMultiplier", 1.f);
    node.var(m_meleeDamageWhenDefendingMultiplier, "meleeDamageWhenDefendingMultiplier", 1.f);
    node.var(m_rangedDamageWhenDefendingMultiplier, "rangedDamageWhenDefendingMultiplier", 1.f);
    node.var(m_maxHPDiff, "maxHPDiff", {});
    node.var(m_recoilMultiplier, "recoilMultiplier", 1.f);
    node.var(m_minProjectilesSpreadAngleMultiplier, "minProjectilesSpreadAngleMultiplier", 1.f);
    node.var(m_jumpVelocityMultiplier, "jumpVelocityMultiplier", 1.f);
    node.var(m_HPRegenerationPer5SecDiff, "HPRegenerationPer5SecDiff", {});

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        if(m_moveSpeedMultiplier < 0.f)
            throw engine::Exception{"Move speed multiplier can't be negative."};

        if(m_fallDamageMultiplier < 0.f)
            throw engine::Exception{"Fall damage multiplier can't be negative."};

        if(m_meleeDamageMultiplier < 0.f)
            throw engine::Exception{"Melee damage multiplier can't be negative."};

        if(m_rangedDamageMultiplier < 0.f)
            throw engine::Exception{"Ranged damage multiplier can't be negative."};

        if(m_meleeDamageWhenDefendingMultiplier < 0.f)
            throw engine::Exception{"Melee damage when defending multiplier can't be negative."};

        if(m_rangedDamageWhenDefendingMultiplier < 0.f)
            throw engine::Exception{"Ranged damage when defending multiplier can't be negative."};

        if(m_recoilMultiplier < 0.f)
            throw engine::Exception{"Recoil multiplier can't be negative."};

        if(m_minProjectilesSpreadAngleMultiplier < 0.f)
            throw engine::Exception{"Min projectiles spread angle multiplier can't be negative."};

        if(m_jumpVelocityMultiplier < 0.f)
            throw engine::Exception{"Jump velocity multiplier can't be negative."};

        if(m_HPRegenerationPer5SecDiff < 0)
            throw engine::Exception{"HP regeneration can't be negative."};
    }
}

int CharacterStatsChange::drawEffects(const engine::IntVec2 &pos) const
{
    auto pos2 = pos;
    auto &renderer = Global::getCore().getDevice().getGUIManager().getRenderer();

    if(m_moveSpeedMultiplier != 1.f) {
        renderer.drawText("Move speed: x" + engine::StringUtility::toIntPercentString(m_moveSpeedMultiplier), pos2, {0.8f, 0.8f, 0.3f});
        pos2.y += 12;
    }

    if(m_fallDamageMultiplier != 1.f) {
        renderer.drawText("Fall damage: x" + engine::StringUtility::toIntPercentString(m_fallDamageMultiplier), pos2, {0.8f, 0.8f, 0.3f});
        pos2.y += 12;
    }

    if(m_meleeDamageMultiplier != 1.f) {
        renderer.drawText("Melee damage: x" + engine::StringUtility::toIntPercentString(m_meleeDamageMultiplier), pos2, {0.8f, 0.8f, 0.3f});
        pos2.y += 12;
    }

    if(m_rangedDamageMultiplier != 1.f) {
        renderer.drawText("Ranged damage: x" + engine::StringUtility::toIntPercentString(m_rangedDamageMultiplier), pos2, {0.8f, 0.8f, 0.3f});
        pos2.y += 12;
    }

    if(m_meleeDamageWhenDefendingMultiplier != 1.f) {
        renderer.drawText("Melee damage when defending: x" + engine::StringUtility::toIntPercentString(m_meleeDamageWhenDefendingMultiplier), pos2, {0.8f, 0.8f, 0.3f});
        pos2.y += 12;
    }

    if(m_rangedDamageWhenDefendingMultiplier != 1.f) {
        renderer.drawText("Ranged damage when defending: x" + engine::StringUtility::toIntPercentString(m_rangedDamageWhenDefendingMultiplier), pos2, {0.8f, 0.8f, 0.3f});
        pos2.y += 12;
    }

    if(m_maxHPDiff) {
        renderer.drawText("Max HP: " + engine::StringUtility::toStringWithSign(m_maxHPDiff), pos2, {0.8f, 0.8f, 0.3f});
        pos2.y += 12;
    }

    if(m_recoilMultiplier != 1.f) {
        renderer.drawText("Recoil: x" + engine::StringUtility::toIntPercentString(m_recoilMultiplier), pos2, {0.8f, 0.8f, 0.3f});
        pos2.y += 12;
    }

    if(m_minProjectilesSpreadAngleMultiplier != 1.f) {
        renderer.drawText("Projectiles spread angle: x" + engine::StringUtility::toIntPercentString(m_minProjectilesSpreadAngleMultiplier), pos2, {0.8f, 0.8f, 0.3f});
        pos2.y += 12;
    }

    if(m_jumpVelocityMultiplier != 1.f) {
        renderer.drawText("Jump velocity: x" + engine::StringUtility::toIntPercentString(m_jumpVelocityMultiplier), pos2, {0.8f, 0.8f, 0.3f});
        pos2.y += 12;
    }

    if(m_HPRegenerationPer5SecDiff) {
        renderer.drawText("HP regeneration per 5 sec: " + engine::StringUtility::toStringWithSign(m_HPRegenerationPer5SecDiff), pos2, {0.8f, 0.8f, 0.3f});
        pos2.y += 12;
    }

    return pos2.y - pos.y;
}

float CharacterStatsChange::getMoveSpeedMultiplier() const
{
    return m_moveSpeedMultiplier;
}

float CharacterStatsChange::getFallDamageMultiplier() const
{
    return m_fallDamageMultiplier;
}

float CharacterStatsChange::getMeleeDamageMultiplier() const
{
    return m_meleeDamageMultiplier;
}

float CharacterStatsChange::getRangedDamageMultiplier() const
{
    return m_rangedDamageMultiplier;
}

float CharacterStatsChange::getMeleeDamageWhenDefendingMultiplier() const
{
    return m_meleeDamageWhenDefendingMultiplier;
}

float CharacterStatsChange::getRangedDamageWhenDefendingMultiplier() const
{
    return m_rangedDamageWhenDefendingMultiplier;
}

int CharacterStatsChange::getMaxHPDiff() const
{
    return m_maxHPDiff;
}

float CharacterStatsChange::getRecoilMultiplier() const
{
    return m_recoilMultiplier;
}

float CharacterStatsChange::getMinProjectilesSpreadAngleMultiplier() const
{
    return m_minProjectilesSpreadAngleMultiplier;
}

float CharacterStatsChange::getJumpVelocityMultiplier() const
{
    return m_jumpVelocityMultiplier;
}

int CharacterStatsChange::getHPRegenerationPer5SecDiff() const
{
    return m_HPRegenerationPer5SecDiff;
}

} // namespace app
