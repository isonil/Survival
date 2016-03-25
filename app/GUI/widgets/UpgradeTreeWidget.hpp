#ifndef APP_UPGRADE_TREE_WIDGET_HPP
#define APP_UPGRADE_TREE_WIDGET_HPP

#include "../../defs/UpgradeDef.hpp"
#include "engine/GUI/widgets/Widget.hpp"
#include "engine/util/Color.hpp"

namespace app
{

class Character;

class UpgradeTreeWidget : public engine::GUI::Widget, public engine::Tracked <UpgradeTreeWidget>
{
public:
    UpgradeTreeWidget(engine::GUI::WidgetContainer *parent, const std::shared_ptr <engine::GUI::IGUIRenderer> &renderer, const engine::IntRect &rect, const std::shared_ptr <Character> &character);

    void onEvent(engine::GUI::Event &event) override;
    void update(const engine::AppTime &time, bool nonOccludedMouseOver) override;
    void draw() const override;
    bool anyNonInternalChildWidgets() const override;

private:
    using base = Widget;

    void drawConnection(const UpgradeDef &upgradeDef, const engine::Color &color) const;
    void movePosAccordingToDir(engine::IntVec2 &pos, const UpgradeDef::RequiredUpgrade::Direction &dir) const;
    std::shared_ptr <UpgradeDef> getUpgradeDefAt(const engine::IntVec2 &cursorPos) const;
    bool canTryUnlock(const UpgradeDef &upgradeDef) const;

    static const engine::IntVec2 k_upgradeSize;

    std::shared_ptr <Character> m_character;
    std::shared_ptr <UpgradeDef> m_currentlyClickedUpgradeDef;
};

} // namespace app

#endif // APP_UPGRADE_TREE_WIDGET_HPP
