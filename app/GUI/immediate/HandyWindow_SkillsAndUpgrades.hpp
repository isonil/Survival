#ifndef APP_HANDY_WINDOW_SKILLS_AND_UPGRADES_HPP
#define APP_HANDY_WINDOW_SKILLS_AND_UPGRADES_HPP

#include "HandyWindow.hpp"
#include "ExperienceBar.hpp"

namespace app
{

class HandyWindow_SkillsAndUpgrades : public HandyWindow
{
public:
    HandyWindow_SkillsAndUpgrades();

    void update() override;
    void onEvent(engine::GUI::Event &event) override;
    void draw() override;
    Type getType() const override;

private:
    using base = HandyWindow;

    static const engine::IntVec2 k_size;

    ExperienceBar m_xp;
    ExperienceBar m_constructing;
    ExperienceBar m_shooting;
    ExperienceBar m_crafting;
    ExperienceBar m_electronics;
};

} // namespace app

#endif // APP_HANDY_WINDOW_SKILLS_AND_UPGRADES_HPP
