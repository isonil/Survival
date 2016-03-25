#include "CharacterStatsAccumulator.hpp"

namespace app
{

CharacterStatsAccumulator::CharacterStatsAccumulator()
    : m_cachedMoveSpeedMultiplier{1.f},
      m_cachedFallDamageMultiplier{1.f},
      m_cachedMeleeDamageMultiplier{1.f},
      m_cachedRangedDamageMultiplier{1.f},
      m_cachedMeleeDamageWhenDefendingMultiplier{1.f},
      m_cachedRangedDamageWhenDefendingMultiplier{1.f},
      m_cachedMaxHPDiff{},
      m_cachedRecoilMutliplier{1.f},
      m_cachedMinProjectilesSpreadAngleMultiplier{1.f},
      m_cachedJumpVelocityMultiplier{1.f},
      m_cachedHPRegenerationPer5SecDiff{}
{
}

void CharacterStatsAccumulator::add(const CharacterStatsChange &stats)
{
    m_stats.push_back(stats);
    recacheValues();
}

void CharacterStatsAccumulator::remove(const CharacterStatsChange &stats)
{
    const auto &it = std::find(m_stats.begin(), m_stats.end(), stats);

    if(it != m_stats.end()) {
        m_stats.erase(it);
        recacheValues();
    }
}

float CharacterStatsAccumulator::getMoveSpeedMultiplier() const
{
    return m_cachedMoveSpeedMultiplier;
}

float CharacterStatsAccumulator::getFallDamageMultiplier() const
{
    return m_cachedFallDamageMultiplier;
}

float CharacterStatsAccumulator::getMeleeDamageMultiplier() const
{
    return m_cachedMeleeDamageMultiplier;
}

float CharacterStatsAccumulator::getRangedDamageMultiplier() const
{
    return m_cachedRangedDamageMultiplier;
}

float CharacterStatsAccumulator::getMeleeDamageWhenDefendingMultiplier() const
{
    return m_cachedMeleeDamageWhenDefendingMultiplier;
}

float CharacterStatsAccumulator::getRangedDamageWhenDefendingMultiplier() const
{
    return m_cachedRangedDamageWhenDefendingMultiplier;
}

int CharacterStatsAccumulator::getMaxHPDiff() const
{
    return m_cachedMaxHPDiff;
}

float CharacterStatsAccumulator::getRecoilMultiplier() const
{
    return m_cachedRecoilMutliplier;
}

float CharacterStatsAccumulator::getMinProjectilesSpreadAngleMultiplier() const
{
    return m_cachedMinProjectilesSpreadAngleMultiplier;
}

float CharacterStatsAccumulator::getJumpVelocityMultiplier() const
{
    return m_cachedJumpVelocityMultiplier;
}

int CharacterStatsAccumulator::getHPRegenerationPer5SecDiff() const
{
    return m_cachedHPRegenerationPer5SecDiff;
}

void CharacterStatsAccumulator::recacheValues()
{
    m_cachedMoveSpeedMultiplier = 1.f;
    m_cachedFallDamageMultiplier = 1.f;
    m_cachedMeleeDamageMultiplier = 1.f;
    m_cachedRangedDamageMultiplier = 1.f;
    m_cachedMeleeDamageWhenDefendingMultiplier = 1.f;
    m_cachedRangedDamageWhenDefendingMultiplier = 1.f;
    m_cachedMaxHPDiff = 0;
    m_cachedRecoilMutliplier = 1.f;
    m_cachedMinProjectilesSpreadAngleMultiplier = 1.f;
    m_cachedJumpVelocityMultiplier = 1.f;
    m_cachedHPRegenerationPer5SecDiff = 0;

    for(const auto &elem : m_stats) {
        m_cachedMoveSpeedMultiplier *= elem.getMoveSpeedMultiplier();
        m_cachedFallDamageMultiplier *= elem.getFallDamageMultiplier();
        m_cachedMeleeDamageMultiplier *= elem.getMeleeDamageMultiplier();
        m_cachedRangedDamageMultiplier *= elem.getRangedDamageMultiplier();
        m_cachedMeleeDamageWhenDefendingMultiplier *= elem.getMeleeDamageWhenDefendingMultiplier();
        m_cachedRangedDamageWhenDefendingMultiplier *= elem.getRangedDamageWhenDefendingMultiplier();
        m_cachedMaxHPDiff += elem.getMaxHPDiff();
        m_cachedRecoilMutliplier *= elem.getRecoilMultiplier();
        m_cachedMinProjectilesSpreadAngleMultiplier *= elem.getMinProjectilesSpreadAngleMultiplier();
        m_cachedJumpVelocityMultiplier *= elem.getJumpVelocityMultiplier();
        m_cachedHPRegenerationPer5SecDiff += elem.getHPRegenerationPer5SecDiff();
    }
}

} // namespace app
