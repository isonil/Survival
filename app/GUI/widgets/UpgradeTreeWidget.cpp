#include "UpgradeTreeWidget.hpp"

#include "../../entities/components/player/UnlockedUpgrades.hpp"
#include "../../entities/components/PlayerComponent.hpp"
#include "../../entities/Character.hpp"
#include "../../thisPlayer/ThisPlayer.hpp"
#include "../../defs/DefsCache.hpp"
#include "../../Global.hpp"
#include "../../Core.hpp"
#include "../../SoundPool.hpp"
#include "engine/GUI/IGUIRenderer.hpp"
#include "engine/GUI/IGUITexture.hpp"
#include "engine/GUI/Event.hpp"
#include "../MainGUI.hpp"

namespace app
{

UpgradeTreeWidget::UpgradeTreeWidget(engine::GUI::WidgetContainer *parent, const std::shared_ptr <engine::GUI::IGUIRenderer> &renderer, const engine::IntRect &rect, const std::shared_ptr <Character> &character)
    : Widget{parent, renderer, rect},
      m_character{character}
{
    if(!m_character)
        throw engine::Exception{"Character is nullptr."};
}

void UpgradeTreeWidget::onEvent(engine::GUI::Event &event)
{
    base::onEvent(event);

    if(event.getType() == engine::GUI::Event::Type::MouseEvent &&
       event.getMouseButton() == engine::GUI::Event::MouseButton::Left) {
        if(event.getMouseAction() == engine::GUI::Event::MouseAction::Down) {
            const auto &upgradeDef = getUpgradeDefAt(event.getPosition());

            if(upgradeDef && canTryUnlock(*upgradeDef))
                m_currentlyClickedUpgradeDef = upgradeDef;
        }
        else if(event.getMouseAction() == engine::GUI::Event::MouseAction::Up) {
            const auto &upgradeDef = getUpgradeDefAt(event.getPosition());

            if(upgradeDef && upgradeDef == m_currentlyClickedUpgradeDef && canTryUnlock(*upgradeDef)) {
                auto &unlockedUpgrades = Global::getCore().getThisPlayer().getCharacter().getPlayerComponent().getUnlockedUpgrades();

                if(unlockedUpgrades.tryUnlock(upgradeDef)) {
                    auto &core = Global::getCore();
                    auto &soundPool = core.getSoundPool();
                    auto &defsCache = core.getDefsCache();

                    soundPool.play(defsCache.Sound_UnlockUpgrade);
                }
            }

            m_currentlyClickedUpgradeDef.reset();
        }
    }
}

void UpgradeTreeWidget::update(const engine::AppTime &time, bool nonOccludedMouseOver)
{
    const auto &upgradeDefAtMousePos = getUpgradeDefAt(getMousePos());

    if(nonOccludedMouseOver && upgradeDefAtMousePos)
        Global::getCore().getMainGUI().getTooltip().setUpgradeDef(upgradeDefAtMousePos);
}

void UpgradeTreeWidget::draw() const
{
    const auto &screenRect = getScreenRect();
    auto &core = Global::getCore();
    auto &defsCache = core.getDefsCache();
    const auto &unlockedUpgrades = core.getThisPlayer().getCharacter().getPlayerComponent().getUnlockedUpgrades();
    const auto &upgradeDefUnderMouse = getUpgradeDefAt(getMousePos());

    for(const auto &elem : defsCache.AllUpgrades) {
        E_DASSERT(elem, "Upgrade def is nullptr.");

        auto color = engine::Color::k_white;
        auto lineColor = engine::Color::k_black;

        if(elem->getColor() == UpgradeDef::Color::Red)
            color = engine::Color::k_red;
        else if(elem->getColor() == UpgradeDef::Color::Green)
            color = engine::Color::k_green;
        else if(elem->getColor() == UpgradeDef::Color::Blue)
            color = {0.2f, 0.2f, 0.9f};

        bool showUnlockedIcon{true};

        if(!unlockedUpgrades.isUnlocked(elem)) {
            color.a = 0.52f;
            lineColor.a = 0.52f;
            showUnlockedIcon = false;
        }

        drawConnection(*elem, lineColor);

        if(m_currentlyClickedUpgradeDef) {
            if(m_currentlyClickedUpgradeDef == elem)
                showUnlockedIcon = true;
        }
        else if(upgradeDefUnderMouse == elem && canTryUnlock(*elem))
            showUnlockedIcon = true;

        const auto &pos = screenRect.pos + elem->getPosition();
        const auto &texture = showUnlockedIcon ? elem->getUnlockedIconTexturePtr() : elem->getIconTexturePtr();

        E_DASSERT(texture, "Texture is nullptr.");

        const auto &textureSize = texture->getSize();

        texture->draw(pos - textureSize / 2, color);
    }
}

bool UpgradeTreeWidget::anyNonInternalChildWidgets() const
{
    return false;
}

void UpgradeTreeWidget::drawConnection(const UpgradeDef &upgradeDef, const engine::Color &color) const
{
    const auto &requiredUpgrade = upgradeDef.getRequiredUpgrade();

    if(!requiredUpgrade.any())
        return;

    auto fromPos = requiredUpgrade.getUpgradeDef().getPosition();
    auto toPos = upgradeDef.getPosition();
    const auto &fromDir = requiredUpgrade.getFrom();
    const auto &toDir = requiredUpgrade.getTo();
    const auto &renderer = getRenderer();
    const auto &screenPos = getScreenRect().pos;

    movePosAccordingToDir(fromPos, fromDir);
    movePosAccordingToDir(toPos, toDir);

    engine::IntVec2 offset{1, 1};

    using Direction = UpgradeDef::RequiredUpgrade::Direction;

    if((fromDir == Direction::Up && toDir == Direction::Up) ||
       (fromDir == Direction::Up && toDir == Direction::Down) ||
       (fromDir == Direction::Down && toDir == Direction::Up) ||
       (fromDir == Direction::Down && toDir == Direction::Down)) {
        // go to (y/2), (x), (y)

        engine::IntVec2 pos1{fromPos.x, (fromPos.y + toPos.y) / 2};
        engine::IntVec2 pos2{toPos.x, pos1.y};

        renderer.drawLine(screenPos + fromPos + offset, screenPos + pos1 + offset, color.brightened(0.4f));
        renderer.drawLine(screenPos + pos1 + offset, screenPos + pos2 + offset, color.brightened(0.4f));
        renderer.drawLine(screenPos + pos2 + offset, screenPos + toPos + offset, color.brightened(0.4f));

        renderer.drawLine(screenPos + fromPos, screenPos + pos1, color);
        renderer.drawLine(screenPos + pos1, screenPos + pos2, color);
        renderer.drawLine(screenPos + pos2, screenPos + toPos, color);
    }
    else if((fromDir == Direction::Up && toDir == Direction::Left) ||
            (fromDir == Direction::Up && toDir == Direction::Right) ||
            (fromDir == Direction::Down && toDir == Direction::Left) ||
            (fromDir == Direction::Down && toDir == Direction::Right)) {
        // go to (y), (x)

        engine::IntVec2 pos{fromPos.x, toPos.y};

        renderer.drawLine(screenPos + fromPos + offset, screenPos + pos + offset, color.brightened(0.4f));
        renderer.drawLine(screenPos + pos + offset, screenPos + toPos + offset, color.brightened(0.4f));

        renderer.drawLine(screenPos + fromPos, screenPos + pos, color);
        renderer.drawLine(screenPos + pos, screenPos + toPos, color);
    }
    else if((fromDir == Direction::Left && toDir == Direction::Left) ||
            (fromDir == Direction::Left && toDir == Direction::Right) ||
            (fromDir == Direction::Right && toDir == Direction::Left) ||
            (fromDir == Direction::Right && toDir == Direction::Right)) {
        // go to (x/2), (y), (x)

        engine::IntVec2 pos1{(fromPos.x + toPos.x) / 2, fromPos.y};
        engine::IntVec2 pos2{pos1.x, toPos.y};

        renderer.drawLine(screenPos + fromPos + offset, screenPos + pos1 + offset, color.brightened(0.4f));
        renderer.drawLine(screenPos + pos1 + offset, screenPos + pos2 + offset, color.brightened(0.4f));
        renderer.drawLine(screenPos + pos2 + offset, screenPos + toPos + offset, color.brightened(0.4f));

        renderer.drawLine(screenPos + fromPos, screenPos + pos1, color);
        renderer.drawLine(screenPos + pos1, screenPos + pos2, color);
        renderer.drawLine(screenPos + pos2, screenPos + toPos, color);
    }
    else { // left-up, left-down, right-up, right-down
        // go to (x), (y)

        engine::IntVec2 pos{toPos.x, fromPos.y};

        renderer.drawLine(screenPos + fromPos + offset, screenPos + pos + offset, color.brightened(0.4f));
        renderer.drawLine(screenPos + pos + offset, screenPos + toPos + offset, color.brightened(0.4f));

        renderer.drawLine(screenPos + fromPos, screenPos + pos, color);
        renderer.drawLine(screenPos + pos, screenPos + toPos, color);
    }
}

void UpgradeTreeWidget::movePosAccordingToDir(engine::IntVec2 &pos, const UpgradeDef::RequiredUpgrade::Direction &dir) const
{
    using Direction = UpgradeDef::RequiredUpgrade::Direction;

    if(dir == Direction::Up)
        pos.y -= k_upgradeSize.y / 2;
    else if(dir == Direction::Down)
        pos.y += k_upgradeSize.y / 2;
    else if(dir == Direction::Left)
        pos.x -= k_upgradeSize.x / 2;
    else if(dir == Direction::Right)
        pos.x += k_upgradeSize.x / 2;
}

std::shared_ptr <UpgradeDef> UpgradeTreeWidget::getUpgradeDefAt(const engine::IntVec2 &cursorPos) const
{
    const auto &screenRect = getScreenRect();
    auto &defsCache = Global::getCore().getDefsCache();

    for(const auto &elem : defsCache.AllUpgrades) {
        E_DASSERT(elem, "Upgrade def is nullptr.");

        const auto &pos = screenRect.pos + elem->getPosition() - k_upgradeSize / 2;
        engine::IntRect rect{pos, k_upgradeSize};

        if(rect.contains(cursorPos))
            return elem;
    }

    return nullptr;
}

bool UpgradeTreeWidget::canTryUnlock(const UpgradeDef &upgradeDef) const
{
    const auto &unlockedUpgrades = Global::getCore().getThisPlayer().getCharacter().getPlayerComponent().getUnlockedUpgrades();

    if(unlockedUpgrades.isUnlocked(upgradeDef))
        return false;

    const auto &requiredUpgrade = upgradeDef.getRequiredUpgrade();

    if(!requiredUpgrade.any())
        return true;

    if(unlockedUpgrades.isUnlocked(requiredUpgrade.getUpgradeDefPtr()))
        return true;

    return false;
}

const engine::IntVec2 UpgradeTreeWidget::k_upgradeSize{50, 50};

} // namespace app
