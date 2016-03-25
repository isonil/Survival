#include "SkillsAndUpgradesWindow.hpp"

#include "../../entities/components/PlayerComponent.hpp"
#include "../../entities/Character.hpp"
#include "engine/GUI/widgets/Window.hpp"
#include "engine/GUI/widgets/Label.hpp"
#include "../../Global.hpp"
#include "../../Core.hpp"
#include "../widgets/UpgradeTreeWidget.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/GUI/GUIManager.hpp"
#include "engine/GUI/IGUIRenderer.hpp"
#include "../MainGUI.hpp"

namespace app
{

SkillsAndUpgradesWindow::SkillsAndUpgradesWindow(const std::shared_ptr <Character> &character)
    : m_character{character}
{
    TRACK;

    if(!character)
        throw engine::Exception{"Character is nullptr."};

    auto &device = Global::getCore().getDevice();
    auto &GUIManager = device.getGUIManager();
    const auto &screenSize = device.getScreenSize();

    m_window = GUIManager.addWindow({(screenSize - k_size) / 2, k_size});
    m_window->setTitle(k_title);

    m_upgradeTreeWidget = m_window->addCustomWidget <UpgradeTreeWidget> ({0, 25, k_size.x, k_size.y - 25}, character);
    m_upgradePointsLabel = m_window->addLabel({20, 40, 200, 20});
    m_upgradePointsLabel->setColor(engine::Color::k_white);
}

void SkillsAndUpgradesWindow::update()
{
    E_DASSERT(m_upgradePointsLabel, "Label is nullptr.");
    E_DASSERT(m_character, "Character is nullptr.");

    int upgradePoints{m_character->getPlayerComponent().getUnlockedUpgrades().getUpgradePoints()};
    m_upgradePointsLabel->setText("Available upgrade points: " + std::to_string(upgradePoints));
}

const engine::IntVec2 SkillsAndUpgradesWindow::k_size{850, 650};
const std::string SkillsAndUpgradesWindow::k_title = "Skills and upgrades";

} // namespace app
