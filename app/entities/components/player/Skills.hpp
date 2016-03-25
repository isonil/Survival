#ifndef APP_SKILLS_HPP
#define APP_SKILLS_HPP

#include "engine/util/Trace.hpp"
#include "engine/util/DataFile.hpp"

namespace app
{

class Character;
class PlayerComponent;

class Skills : public engine::DataFile::Saveable, public engine::Tracked <Skills>
{
public:
    Skills(Character &character, PlayerComponent &playerComponent);

    void expose(engine::DataFile::Node &node) override;

    int getLevel() const;
    float getLevelPercentage() const;
    void addExp(int amount);
    int getExp() const;
    int getExpRequiredForNextLevel() const;

    int getConstructingLevel() const;
    float getConstructingLevelPercentage() const;
    void addConstructingExp(int amount);

    int getShootingLevel() const;
    float getShootingLevelPercentage() const;
    void addShootingExp(int amount);

    int getCraftingLevel() const;
    float getCraftingLevelPercentage() const;
    void addCraftingExp(int amount);

    int getElectronicsLevel() const;
    float getElectronicsLevelPercentage() const;
    void addElectronicsExp(int amount);

private:
    struct LevelInfo
    {
        LevelInfo();
        LevelInfo(int level, int requiredThisLevelExp, int requiredNextLevelExp);

        int level;
        int requiredThisLevelExp;
        int requiredNextLevelExp;
    };

    static LevelInfo getLevelFromExp(int exp);

    void recache();
    void leveledUp();

    static const int k_upgradePointsPerLevel;

    Character &m_character;
    PlayerComponent &m_playerComponent;

    int m_exp;
    int m_constructingExp;
    int m_shootingExp;
    int m_craftingExp;
    int m_electronicsExp;

    LevelInfo m_cachedLevel;
    LevelInfo m_cachedConstructingLevel;
    LevelInfo m_cachedShootingLevel;
    LevelInfo m_cachedCraftingLevel;
    LevelInfo m_cachedElectronicsLevel;
};

} // namespace app

#endif // APP_SKILLS_HPP
