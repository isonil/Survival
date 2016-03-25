#include "UpgradeDef.hpp"

#include "engine/GUI/GUIManager.hpp"
#include "engine/GUI/IGUIRenderer.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/util/DefDatabase.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "StructureRecipeDef.hpp"

namespace app
{

ENUM_DEF(UpgradeDef::Color, Color,
    White,
    Red,
    Green,
    Blue);

ENUM_DEF(UpgradeDef::RequiredUpgrade::Direction, Direction,
    Up,
    Down,
    Left,
    Right);

UpgradeDef::RequiredUpgrade::RequiredUpgrade()
    : m_from{Direction::Up},
      m_to{Direction::Up}
{
}

void UpgradeDef::RequiredUpgrade::expose(engine::DataFile::Node &node)
{
    node.var(m_upgradeDef_defName, "upgradeDef");
    node.var(m_from, "from");
    node.var(m_to, "to");

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading)
        m_upgradeDef.reset();
}

void UpgradeDef::RequiredUpgrade::onLoadedAllDefs()
{
    if(!m_upgradeDef_defName.empty()) {
        auto &defDatabase = Global::getCore().getDefDatabase();
        m_upgradeDef = defDatabase.getDef <UpgradeDef> (m_upgradeDef_defName);
    }
}

bool UpgradeDef::RequiredUpgrade::any() const
{
    return static_cast <bool> (m_upgradeDef);
}

const std::shared_ptr <UpgradeDef> &UpgradeDef::RequiredUpgrade::getUpgradeDefPtr() const
{
    if(!m_upgradeDef)
        throw engine::Exception{"Upgrade def is nullptr. This should have been checked before."};

    return m_upgradeDef;
}

UpgradeDef &UpgradeDef::RequiredUpgrade::getUpgradeDef() const
{
    if(!m_upgradeDef)
        throw engine::Exception{"Upgrade def is nullptr. This should have been checked before."};

    return *m_upgradeDef;
}

const UpgradeDef::RequiredUpgrade::Direction &UpgradeDef::RequiredUpgrade::getFrom() const
{
    return m_from;
}

const UpgradeDef::RequiredUpgrade::Direction &UpgradeDef::RequiredUpgrade::getTo() const
{
    return m_to;
}

UpgradeDef::UpgradeDef()
    : m_color{Color::White},
      m_isUnlockedByDefault{},
      m_requiredUpgradePoints{}
{
}

void UpgradeDef::expose(engine::DataFile::Node &node)
{
    TRACK;

    base::expose(node);

    node.var(m_color, "color");
    node.var(m_position, "position");
    node.var(m_isUnlockedByDefault, "isUnlockedByDefault", {});
    node.var(m_requiredUpgradePoints, "requiredUpgradePoints", {});
    node.var(m_requiredUpgrade, "requiredUpgrade", {});
    node.var(m_characterStatsChange, "characterStatsChange", {});
    node.var(m_unlockStructures, "unlockStructures", {});
    node.var(m_iconTexture_path, "icon");
    node.var(m_unlockedIconTexture_path, "unlockedIcon");

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        auto &GUIRenderer = Global::getCore().getDevice().getGUIManager().getRenderer();

        if(m_requiredUpgradePoints < 0)
            throw engine::Exception{"Required upgrade points value can't be negative."};

        m_iconTexture = GUIRenderer.getTexture(m_iconTexture_path);
        m_unlockedIconTexture = GUIRenderer.getTexture(m_unlockedIconTexture_path);
    }
}

void UpgradeDef::onLoadedAllDefs(engine::DefDatabase &defDatabase)
{
    m_requiredUpgrade.onLoadedAllDefs();
}

int UpgradeDef::drawEffects(const engine::IntVec2 &pos) const
{
    auto pos2 = pos;

    pos2.y += m_characterStatsChange.drawEffects(pos);

    auto &renderer = Global::getCore().getDevice().getGUIManager().getRenderer();

    for(const auto &elem : m_unlockStructures.recipeDefs) {
        E_DASSERT(elem, "Structure recipe def is nullptr.");
        renderer.drawText("Unlocks: " + elem->getCapitalizedLabel(), pos2, {0.5f, 0.5f, 1.f});
        pos2.y += 12;
    }

    return pos2.y - pos.y;
}

const UpgradeDef::Color &UpgradeDef::getColor() const
{
    return m_color;
}

const engine::IntVec2 &UpgradeDef::getPosition() const
{
    return m_position;
}

bool UpgradeDef::isUnlockedByDefault() const
{
    return m_isUnlockedByDefault;
}

int UpgradeDef::getRequiredUpgradePoints() const
{
    return m_requiredUpgradePoints;
}

const CharacterStatsChange &UpgradeDef::getCharacterStatsChange() const
{
    return m_characterStatsChange;
}

const UpgradeDef::RequiredUpgrade &UpgradeDef::getRequiredUpgrade() const
{
    return m_requiredUpgrade;
}

const std::vector <std::shared_ptr <StructureRecipeDef>> &UpgradeDef::getUnlockStructureRecipeDefs() const
{
    return m_unlockStructures.recipeDefs;
}

const std::shared_ptr <engine::GUI::IGUITexture> &UpgradeDef::getIconTexturePtr() const
{
    if(!m_iconTexture)
        throw engine::Exception{"Icon texture is nullptr."};

    return m_iconTexture;
}

const std::shared_ptr <engine::GUI::IGUITexture> &UpgradeDef::getUnlockedIconTexturePtr() const
{
    if(!m_unlockedIconTexture)
        throw engine::Exception{"Unlocked icon texture is nullptr."};

    return m_unlockedIconTexture;
}

void UpgradeDef::UnlockStructures::expose(engine::DataFile::Node &node)
{
    node.var(recipeDefs_defNames, "recipeDefs");

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        auto &defDatabase = Global::getCore().getDefDatabase();

        recipeDefs.clear();

        for(const auto &elem : recipeDefs_defNames) {
            recipeDefs.push_back(defDatabase.getDef <StructureRecipeDef> (elem));
        }
    }
}

} // namespace app
