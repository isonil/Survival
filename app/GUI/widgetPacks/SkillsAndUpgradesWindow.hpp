#ifndef APP_SKILLS_AND_UPGRADES_WINDOW_HPP
#define APP_SKILLS_AND_UPGRADES_WINDOW_HPP

#include "engine/util/Vec2.hpp"

#include <memory>

namespace engine { namespace GUI { class Window; class Label; } }

namespace app
{

class Character;
class UpgradeTreeWidget;

class SkillsAndUpgradesWindow
{
public:
    SkillsAndUpgradesWindow(const std::shared_ptr <Character> &character);

    void update();

private:
    static const engine::IntVec2 k_size;
    static const std::string k_title;

    std::shared_ptr <Character> m_character;
    std::shared_ptr <engine::GUI::Window> m_window;
    std::shared_ptr <engine::GUI::Label> m_upgradePointsLabel;
    std::shared_ptr <UpgradeTreeWidget> m_upgradeTreeWidget;
};

} // namespace app

#endif // APP_SKILLS_AND_UPGRADES_WINDOW_HPP

