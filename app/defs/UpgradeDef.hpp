#ifndef APP_UPGRADE_DEF_HPP
#define APP_UPGRADE_DEF_HPP

#include "../entities/character/CharacterStatsChange.hpp"
#include "engine/util/Trace.hpp"
#include "engine/util/Def.hpp"
#include "engine/util/Enum.hpp"
#include "engine/util/Vec2.hpp"

namespace engine { namespace GUI { class IGUITexture; } }

namespace app
{

class StructureRecipeDef;

class UpgradeDef : public engine::Def, public engine::Tracked <UpgradeDef>
{
public:
    ENUM_DECL(Color,
        White,
        Red,
        Green,
        Blue);

    class RequiredUpgrade : public engine::DataFile::Saveable
    {
    public:
        ENUM_DECL(Direction,
            Up,
            Down,
            Left,
            Right);

        RequiredUpgrade();

        void expose(engine::DataFile::Node &node) override;

        void onLoadedAllDefs();

        bool any() const;

        const std::shared_ptr <UpgradeDef> &getUpgradeDefPtr() const;
        UpgradeDef &getUpgradeDef() const;

        const Direction &getFrom() const;
        const Direction &getTo() const;

    private:
        std::string m_upgradeDef_defName;
        std::shared_ptr <UpgradeDef> m_upgradeDef;
        Direction m_from;
        Direction m_to;
    };

    UpgradeDef();

    void expose(engine::DataFile::Node &node) override;
    void onLoadedAllDefs(engine::DefDatabase &defDatabase) override;

    int drawEffects(const engine::IntVec2 &pos) const;

    const Color &getColor() const;
    const engine::IntVec2 &getPosition() const;
    bool isUnlockedByDefault() const;
    int getRequiredUpgradePoints() const;
    const CharacterStatsChange &getCharacterStatsChange() const;
    const RequiredUpgrade &getRequiredUpgrade() const;
    const std::vector <std::shared_ptr <StructureRecipeDef>> &getUnlockStructureRecipeDefs() const;

    const std::shared_ptr <engine::GUI::IGUITexture> &getIconTexturePtr() const;
    const std::shared_ptr <engine::GUI::IGUITexture> &getUnlockedIconTexturePtr() const;

private:
    using base = Def;

    struct UnlockStructures : public engine::DataFile::Saveable
    {
        void expose(engine::DataFile::Node &node) override;

        std::vector <std::string> recipeDefs_defNames;
        std::vector <std::shared_ptr <StructureRecipeDef>> recipeDefs;
    };

    Color m_color;
    engine::IntVec2 m_position;
    bool m_isUnlockedByDefault;
    int m_requiredUpgradePoints;
    RequiredUpgrade m_requiredUpgrade;
    CharacterStatsChange m_characterStatsChange;
    UnlockStructures m_unlockStructures;

    std::string m_iconTexture_path;
    std::shared_ptr <engine::GUI::IGUITexture> m_iconTexture;
    std::string m_unlockedIconTexture_path;
    std::shared_ptr <engine::GUI::IGUITexture> m_unlockedIconTexture;
};

} // namespace app

#endif // APP_UPGRADE_DEF_HPP
