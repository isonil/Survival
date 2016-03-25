#include "Skills.hpp"

#include "../../../GUI/MainGUI.hpp"
#include "../../../defs/DefsCache.hpp"
#include "../../../Global.hpp"
#include "../../../Core.hpp"
#include "../../../SoundPool.hpp"
#include "../../Character.hpp"
#include "engine/util/Math.hpp"
#include "../PlayerComponent.hpp"

namespace app
{

Skills::Skills(Character &character, PlayerComponent &playerComponent)
    : m_character{character},
      m_playerComponent{playerComponent},
      m_exp{},
      m_constructingExp{},
      m_shootingExp{},
      m_craftingExp{},
      m_electronicsExp{}
{
    recache();
}

void Skills::expose(engine::DataFile::Node &node)
{
    node.var(m_exp, "xp");
    node.var(m_constructingExp, "constructingExp");
    node.var(m_shootingExp, "shootingExp");
    node.var(m_craftingExp, "craftingExp");
    node.var(m_electronicsExp, "electronicsExp");

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading)
        recache();
}

int Skills::getLevel() const
{
    return m_cachedLevel.level;
}

float Skills::getLevelPercentage() const
{
    return engine::Math::inverseLerp(m_cachedLevel.requiredThisLevelExp, m_cachedLevel.requiredNextLevelExp, m_exp);
}

void Skills::addExp(int amount)
{
    if(amount <= 0)
        return;

    int levelBefore{m_cachedLevel.level};

    m_exp += amount;
    recache();

    if(m_cachedLevel.level != levelBefore)
        leveledUp();
}

int Skills::getExp() const
{
    return m_exp;
}

int Skills::getExpRequiredForNextLevel() const
{
    return m_cachedLevel.requiredNextLevelExp;
}

int Skills::getConstructingLevel() const
{
    return m_cachedConstructingLevel.level;
}

float Skills::getConstructingLevelPercentage() const
{
    return engine::Math::inverseLerp(m_cachedConstructingLevel.requiredThisLevelExp, m_cachedConstructingLevel.requiredNextLevelExp, m_constructingExp);
}

void Skills::addConstructingExp(int amount)
{
    if(amount <= 0)
        return;

    m_constructingExp += amount;

    recache();
}

int Skills::getShootingLevel() const
{
    return m_cachedShootingLevel.level;
}

float Skills::getShootingLevelPercentage() const
{
    return engine::Math::inverseLerp(m_cachedShootingLevel.requiredThisLevelExp, m_cachedShootingLevel.requiredNextLevelExp, m_shootingExp);
}

void Skills::addShootingExp(int amount)
{
    if(amount <= 0)
        return;

    m_shootingExp += amount;

    recache();
}

int Skills::getCraftingLevel() const
{
    return m_cachedCraftingLevel.level;
}

float Skills::getCraftingLevelPercentage() const
{
    return engine::Math::inverseLerp(m_cachedCraftingLevel.requiredThisLevelExp, m_cachedCraftingLevel.requiredNextLevelExp, m_craftingExp);
}

void Skills::addCraftingExp(int amount)
{
    if(amount <= 0)
        return;

    m_craftingExp += amount;

    recache();
}

int Skills::getElectronicsLevel() const
{
    return m_cachedElectronicsLevel.level;
}

float Skills::getElectronicsLevelPercentage() const
{
    return engine::Math::inverseLerp(m_cachedElectronicsLevel.requiredThisLevelExp, m_cachedElectronicsLevel.requiredNextLevelExp, m_electronicsExp);
}

void Skills::addElectronicsExp(int amount)
{
    if(amount <= 0)
        return;

    m_electronicsExp += amount;

    recache();
}

Skills::LevelInfo::LevelInfo()
    : level{},
      requiredThisLevelExp{},
      requiredNextLevelExp{}
{
}

Skills::LevelInfo::LevelInfo(int level, int requiredThisLevelExp, int requiredNextLevelExp)
    : level{level},
      requiredThisLevelExp{requiredThisLevelExp},
      requiredNextLevelExp{requiredNextLevelExp}
{
}

Skills::LevelInfo Skills::getLevelFromExp(int exp)
{
    static const std::vector <int> requiredExp = { // diff between levels (not total exp)
        0,
        1000,
        1000,
        1000,
        1200,
        1440,
        1728,
        2073,
        2488,
        2985,
        3583,
        4299,
        5159,
        6191,
        7430,
        8916,
        10699,
        12839,
        15407,
        18488,
        22186,
        26623
    };

    int currentLevel{1};
    int previousRequiredExp{};
    int currentRequiredExp{};
    int requiredExpIndex{};

    while(true) {
        previousRequiredExp = currentRequiredExp;
        currentRequiredExp += requiredExp[requiredExpIndex];

        if(exp < currentRequiredExp)
            return {currentLevel - 1, previousRequiredExp, currentRequiredExp};

        if(requiredExpIndex < static_cast <int> (requiredExp.size()) - 1)
            ++requiredExpIndex;

        ++currentLevel;
    }
}

void Skills::recache()
{
    m_cachedLevel = getLevelFromExp(m_exp);
    m_cachedConstructingLevel = getLevelFromExp(m_constructingExp);
    m_cachedShootingLevel = getLevelFromExp(m_shootingExp);
    m_cachedCraftingLevel = getLevelFromExp(m_craftingExp);
    m_cachedElectronicsLevel = getLevelFromExp(m_electronicsExp);
}

void Skills::leveledUp()
{
    auto &core = Global::getCore();
    auto &defsCache = core.getDefsCache();
    auto &soundPool = core.getSoundPool();
    auto &mainGUI = core.getMainGUI();

    soundPool.play(defsCache.Sound_LevelUp);
    mainGUI.getLevelUpAnimation().show();
    m_playerComponent.getUnlockedUpgrades().addUpgradePoints(k_upgradePointsPerLevel);
}

const int Skills::k_upgradePointsPerLevel{3};

} // namespace app
