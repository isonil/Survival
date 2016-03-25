#include "HandyWindow_SkillsAndUpgrades.hpp"

#include "../../entities/components/PlayerComponent.hpp"
#include "../../entities/Character.hpp"
#include "../../thisPlayer/ThisPlayer.hpp"
#include "../../Core.hpp"
#include "../../Global.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/GUI/GUIManager.hpp"
#include "engine/GUI/IGUIRenderer.hpp"
#include "engine/util/Color.hpp"

namespace app
{

HandyWindow_SkillsAndUpgrades::HandyWindow_SkillsAndUpgrades()
    : HandyWindow{k_size}
{
    const auto &pos = Global::getCore().getDevice().getScreenSize() - k_size;

    m_xp.setPosition(pos.moved(25, 51));
    m_constructing.setPosition(pos.moved(25, 116));
    m_shooting.setPosition(pos.moved(25, 170));
    m_crafting.setPosition(pos.moved(25, 224));
    m_electronics.setPosition(pos.moved(25, 278));

    m_xp.setTitle("Level");
    m_constructing.setTitle("Constructing");
    m_shooting.setTitle("Shooting");
    m_crafting.setTitle("Crafting");
    m_electronics.setTitle("Electronics");
}

void HandyWindow_SkillsAndUpgrades::update()
{
    const auto &skills = Global::getCore().getThisPlayer().getCharacter().getPlayerComponent().getSkills();

    m_xp.setProgress(skills.getLevelPercentage());
    m_xp.setLevel(skills.getLevel());

    m_constructing.setProgress(skills.getConstructingLevelPercentage());
    m_constructing.setLevel(skills.getConstructingLevel());

    m_shooting.setProgress(skills.getShootingLevelPercentage());
    m_shooting.setLevel(skills.getShootingLevel());

    m_crafting.setProgress(skills.getCraftingLevelPercentage());
    m_crafting.setLevel(skills.getCraftingLevel());

    m_electronics.setProgress(skills.getElectronicsLevelPercentage());
    m_electronics.setLevel(skills.getElectronicsLevel());
}

void HandyWindow_SkillsAndUpgrades::onEvent(engine::GUI::Event &event)
{
    base::onEvent(event);
}

void HandyWindow_SkillsAndUpgrades::draw()
{
    base::draw();

    auto &core = Global::getCore();
    auto &device = core.getDevice();
    const auto &pos = device.getScreenSize() - k_size;
    const auto &GUIRenderer = device.getGUIManager().getRenderer();
    const auto &thisPlayerCharacter = core.getThisPlayer().getCharacter();
    const auto &skills = thisPlayerCharacter.getPlayerComponent().getSkills();

    GUIRenderer.drawText("Skills and upgrades", pos, engine::Color::k_white);

    GUIRenderer.drawFilledRect({pos.movedY(21), {k_size.x, 65}}, {1.f, 1.f, 0.3f, 0.15f});

    m_xp.draw();
    m_constructing.draw();
    m_shooting.draw();
    m_crafting.draw();
    m_electronics.draw();

    GUIRenderer.drawText(std::to_string(skills.getExp()) + " / " + std::to_string(skills.getExpRequiredForNextLevel()),
                         {pos.moved(25, 26), {260, 20}}, engine::Color::k_white, engine::GUI::IGUIRenderer::FontSize::Normal,
                         engine::GUI::IGUIRenderer::HorizontalTextAlign::Right);

    int upgradePoints{thisPlayerCharacter.getPlayerComponent().getUnlockedUpgrades().getUpgradePoints()};

    if(upgradePoints)
        GUIRenderer.drawText("Available upgrade points: " + std::to_string(upgradePoints), pos.moved(15, k_size.y - 75), {0.8f, 0.8f, 0.8f});

    //GUIRenderer.drawText("Available skill points: 1", pos.moved(15, k_size.y - 85), {0.8f, 0.8f, 0.8f});
}

HandyWindow::Type HandyWindow_SkillsAndUpgrades::getType() const
{
    return Type::SkillsAndUpgrades;
}

const engine::IntVec2 HandyWindow_SkillsAndUpgrades::k_size{300, 400};

} // namespace app
