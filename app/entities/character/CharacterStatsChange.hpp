#ifndef APP_CHARACTER_STATS_CHANGE_HPP
#define APP_CHARACTER_STATS_CHANGE_HPP

#include "engine/util/DataFile.hpp"
#include "engine/util/Vec2.hpp"

namespace app
{

class CharacterStatsChange : public engine::DataFile::Saveable
{
public:
    CharacterStatsChange();

    bool operator == (const CharacterStatsChange &rhs) const;
    bool operator != (const CharacterStatsChange &rhs) const;

    void expose(engine::DataFile::Node &node) override;

    int drawEffects(const engine::IntVec2 &pos) const;

    float getMoveSpeedMultiplier() const;
    float getFallDamageMultiplier() const;
    float getMeleeDamageMultiplier() const;
    float getRangedDamageMultiplier() const;
    float getMeleeDamageWhenDefendingMultiplier() const;
    float getRangedDamageWhenDefendingMultiplier() const;
    int getMaxHPDiff() const;
    float getRecoilMultiplier() const;
    float getMinProjectilesSpreadAngleMultiplier() const;
    float getJumpVelocityMultiplier() const;
    int getHPRegenerationPer5SecDiff() const;

private:
    float m_moveSpeedMultiplier;
    float m_fallDamageMultiplier;
    float m_meleeDamageMultiplier;
    float m_rangedDamageMultiplier;
    float m_meleeDamageWhenDefendingMultiplier;
    float m_rangedDamageWhenDefendingMultiplier;
    int m_maxHPDiff;
    float m_recoilMultiplier;
    float m_minProjectilesSpreadAngleMultiplier;
    float m_jumpVelocityMultiplier;
    int m_HPRegenerationPer5SecDiff;
};

} // namespace app

#endif // APP_CHARACTER_STATS_CHANGE_HPP
