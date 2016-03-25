#include "CharacterStatsOrSkillsRelatedFormulas.hpp"

#include "../../defs/ItemDef.hpp"
#include "../../defs/CharacterDef.hpp"
#include "../components/PlayerComponent.hpp"
#include "CharacterStatsAccumulator.hpp"
#include "../Character.hpp"
#include "../Item.hpp"

namespace app
{

int CharacterStatsOrSkillsRelatedFormulas::getDamage(const Entity &attacker, const Entity &attacked, const Item &weapon)
{
    const auto &onUsed = weapon.getDef().getOnUsed();

    if(!onUsed.isWeapon())
        return 0;

    auto *attackerCharacter = dynamic_cast <const Character*> (&attacker);
    auto *attackedCharacter = dynamic_cast <const Character*> (&attacked);

    int damage{};

    if(onUsed.isMelee()) {
        float defence{1.f};

        if(attackedCharacter)
            defence = attackedCharacter->getCharacterStatsAccumulator().getMeleeDamageWhenDefendingMultiplier();

        int attackerDamage{};

        if(attackerCharacter)
            attackerDamage = getMeleeDamage(*attackerCharacter, onUsed.getDealsDamage());
        else
            attackerDamage = onUsed.getDealsDamage();

        damage = defence * attackerDamage;
    }
    else {
        float defence{1.f};

        if(attackedCharacter)
            defence = attackedCharacter->getCharacterStatsAccumulator().getRangedDamageWhenDefendingMultiplier();

        int attackerDamage{};

        if(attackerCharacter)
            attackerDamage = getRangedDamage(*attackerCharacter, onUsed.getDealsDamage());
        else
            attackerDamage = onUsed.getDealsDamage();

        damage = defence * attackerDamage;
    }

    return std::max(0, damage);
}

float CharacterStatsOrSkillsRelatedFormulas::getRecoil(const Character &character, const Item &weapon)
{
    const auto &onUsed = weapon.getDef().getOnUsed();

    if(!onUsed.isWeapon() || onUsed.isMelee())
        return 0.f;

    const auto &characterStatsAccumulator = character.getCharacterStatsAccumulator();

    float val{characterStatsAccumulator.getRecoilMultiplier()};
    float val2{1.f};

    if(character.isPlayer()) {
        int level{std::min(50, character.getPlayerComponent().getSkills().getShootingLevel())};
        val2 = 2.f - std::pow(level, k_shootingLevelRecoilPowFactor);

        if(val2 < 0.f)
            val2 = 0.f;
    }

    return onUsed.getRecoil() * val * val2;
}

float CharacterStatsOrSkillsRelatedFormulas::getMinProjectilesSpreadAngle(const Character &character, const Item &weapon)
{
    const auto &onUsed = weapon.getDef().getOnUsed();

    if(!onUsed.isWeapon() || onUsed.isMelee())
        return 0.f;

    const auto &characterStatsAccumulator = character.getCharacterStatsAccumulator();

    float val{characterStatsAccumulator.getMinProjectilesSpreadAngleMultiplier()};

    return onUsed.getMinProjectilesSpreadAngle() * val;
}

int CharacterStatsOrSkillsRelatedFormulas::getMaxHP(const Character &character)
{
    const auto &characterStatsAccumulator = character.getCharacterStatsAccumulator();

    int val{characterStatsAccumulator.getMaxHPDiff()};

    return std::max(1, character.getDef().getMaxHP() + val);
}

float CharacterStatsOrSkillsRelatedFormulas::getJumpVelocity(const Character &character)
{
    const auto &characterStatsAccumulator = character.getCharacterStatsAccumulator();

    float val{characterStatsAccumulator.getJumpVelocityMultiplier()};

    return k_baseJumpVelocity * val;
}

int CharacterStatsOrSkillsRelatedFormulas::getHPRegenerationPer5Sec(const Character &character)
{
    const auto &characterStatsAccumulator = character.getCharacterStatsAccumulator();

    return characterStatsAccumulator.getHPRegenerationPer5SecDiff();
}

int CharacterStatsOrSkillsRelatedFormulas::getFallDamage(const Character &character, float force)
{
    const auto &characterStatsAccumulator = character.getCharacterStatsAccumulator();

    float val{engine::Math::inverseLerp(k_fallDamageForceRange.from, k_fallDamageForceRange.to, force)};
    val = engine::Math::clamp01(val);

    auto damage = static_cast <int> (val * k_maxFallDamage * characterStatsAccumulator.getFallDamageMultiplier());

    return damage;
}

float CharacterStatsOrSkillsRelatedFormulas::getMoveSpeed(const Character &character)
{
    const auto &characterStatsAccumulator = character.getCharacterStatsAccumulator();

    float takenDamageSlowdown{engine::Math::inverseLerp(0.f, k_takenDamageSlowdownTime, character.getMsSinceLastTakenDamage())};
    takenDamageSlowdown = engine::Math::clamp01(takenDamageSlowdown);
    takenDamageSlowdown = engine::Math::lerp(k_takenDamageSlowdownMoveSpeedMultiplier, 1.f, takenDamageSlowdown);

    return k_baseMoveSpeed * characterStatsAccumulator.getMoveSpeedMultiplier() * takenDamageSlowdown;
}

int CharacterStatsOrSkillsRelatedFormulas::getMeleeDamage(const Character &character, int baseWeaponDamage)
{
    const auto &characterStatsAccumulator = character.getCharacterStatsAccumulator();

    float val{characterStatsAccumulator.getMeleeDamageMultiplier()};

    return static_cast <int> (baseWeaponDamage * val);
}

int CharacterStatsOrSkillsRelatedFormulas::getRangedDamage(const Character &character, int baseWeaponDamage)
{
    const auto &characterStatsAccumulator = character.getCharacterStatsAccumulator();

    float val{characterStatsAccumulator.getRangedDamageMultiplier()};
    float val2{1.f};

    if(character.isPlayer()) {
        int level{character.getPlayerComponent().getSkills().getShootingLevel()};
        val2 = std::pow(level, k_shootingLevelRangedDamagePowFactor);
    }

    return static_cast <int> (baseWeaponDamage * val * val2);
}

const float CharacterStatsOrSkillsRelatedFormulas::k_baseMoveSpeed{7.32f};
const int CharacterStatsOrSkillsRelatedFormulas::k_maxFallDamage{100};
const float CharacterStatsOrSkillsRelatedFormulas::k_takenDamageSlowdownTime{1000.f};
const float CharacterStatsOrSkillsRelatedFormulas::k_takenDamageSlowdownMoveSpeedMultiplier{0.5f};
const float CharacterStatsOrSkillsRelatedFormulas::k_shootingLevelRangedDamagePowFactor{0.03f};
const float CharacterStatsOrSkillsRelatedFormulas::k_shootingLevelRecoilPowFactor{0.035f};
const float CharacterStatsOrSkillsRelatedFormulas::k_baseJumpVelocity{8.5f};
const engine::FloatRange CharacterStatsOrSkillsRelatedFormulas::k_fallDamageForceRange{18.f, 50.f};

} // namespace app
