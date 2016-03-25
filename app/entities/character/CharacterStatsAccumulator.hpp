#ifndef APP_STATS_ACCUMULATOR_HPP
#define APP_STATS_ACCUMULATOR_HPP

#include "CharacterStatsChange.hpp"

#include <vector>

namespace app
{

// Note: this class should not be serializable
// (because stats added to this object could be different after deserialization)

class CharacterStatsAccumulator : public engine::Tracked <CharacterStatsAccumulator>
{
public:
    CharacterStatsAccumulator();

    void add(const CharacterStatsChange &stats);
    void remove(const CharacterStatsChange &stats);

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
    void recacheValues();

    std::vector <CharacterStatsChange> m_stats;

    float m_cachedMoveSpeedMultiplier;
    float m_cachedFallDamageMultiplier;
    float m_cachedMeleeDamageMultiplier;
    float m_cachedRangedDamageMultiplier;
    float m_cachedMeleeDamageWhenDefendingMultiplier;
    float m_cachedRangedDamageWhenDefendingMultiplier;
    int m_cachedMaxHPDiff;
    float m_cachedRecoilMutliplier;
    float m_cachedMinProjectilesSpreadAngleMultiplier;
    float m_cachedJumpVelocityMultiplier;
    int m_cachedHPRegenerationPer5SecDiff;
};

} // namespace app

#endif // APP_STATS_ACCUMULATOR_HPP
