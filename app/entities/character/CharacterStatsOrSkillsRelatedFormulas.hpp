#ifndef APP_CHARACTER_STATS_OR_SKILLS_RELATED_FORMULAS_HPP
#define APP_CHARACTER_STATS_OR_SKILLS_RELATED_FORMULAS_HPP

#include "engine/util/Range.hpp"

namespace app
{

class Entity;
class Character;
class Item;

class CharacterStatsOrSkillsRelatedFormulas
{
public:
    static int getDamage(const Entity &attacker, const Entity &attacked, const Item &weapon);
    static float getRecoil(const Character &character, const Item &weapon);
    static float getMinProjectilesSpreadAngle(const Character &character, const Item &weapon);
    static int getMaxHP(const Character &character);
    static float getJumpVelocity(const Character &character);
    static int getHPRegenerationPer5Sec(const Character &character);
    static int getFallDamage(const Character &character, float force);
    static float getMoveSpeed(const Character &character);

private:
    static int getMeleeDamage(const Character &character, int baseWeaponDamage);
    static int getRangedDamage(const Character &character, int baseWeaponDamage);

    static const float k_baseMoveSpeed;
    static const int k_maxFallDamage;
    static const float k_takenDamageSlowdownTime;
    static const float k_takenDamageSlowdownMoveSpeedMultiplier;
    static const float k_shootingLevelRangedDamagePowFactor;
    static const float k_shootingLevelRecoilPowFactor;
    static const float k_baseJumpVelocity;
    static const engine::FloatRange k_fallDamageForceRange;
};

} // namespace app

#endif // APP_CHARACTER_STATS_OR_SKILLS_RELATED_FORMULAS_HPP
