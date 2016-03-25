#include "MainGUI.hpp"

#include "../entities/character/ProjectilesSpreadAngleManager.hpp"
#include "engine/app3D/managers/CursorManager.hpp"
#include "engine/app3D/managers/SceneManager.hpp"
#include "engine/GUI/widgets/RectWidget.hpp"
#include "engine/GUI/widgets/Button.hpp"
#include "engine/GUI/widgets/Label.hpp"
#include "engine/GUI/widgets/Image.hpp"
#include "engine/GUI/widgets/ProgressBar.hpp"
#include "engine/GUI/GUIManager.hpp"
#include "engine/GUI/IGUITexture.hpp"
#include "engine/GUI/IGUIRenderer.hpp"
#include "../entities/Item.hpp"
#include "../entities/Character.hpp"
#include "../entities/Structure.hpp"
#include "../defs/ItemDef.hpp"
#include "../defs/DefsCache.hpp"
#include "../thisPlayer/ThisPlayer.hpp"
#include "../world/World.hpp"
#include "../world/ElectricitySystem.hpp"
#include "engine/app3D/Device.hpp"
#include "immediate/HandyWindow_Construct.hpp"
#include "immediate/HandyWindow_SkillsAndUpgrades.hpp"
#include "immediate/HandyWindow_Inventory.hpp"
#include "widgets/MultiSlotItemContainerWidget.hpp"
#include "widgetPacks/InventoryWindow.hpp"
#include "widgetPacks/SkillsAndUpgradesWindow.hpp"
#include "widgetPacks/ItemContainerWindow.hpp"
#include "widgetPacks/CraftingWindow.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "../SoundPool.hpp"

namespace app
{

MainGUI::MainGUI()
    : m_GUIMode{},
      m_crosshairVisible{true},
      m_highlightAlpha{0.f, InterpolationType::FixedStepLinear, k_highlightAlphaInterpolationStep},
      m_crosshairAlpha{0.f, InterpolationType::FixedStepLinear, k_crosshairAlphaInterpolationStep},
      m_hitTargetAlpha{0.f, InterpolationType::FixedStepLinear, k_hitTargetAlphaInterpolationStep},
      m_screenFadeIn{1.f, InterpolationType::FixedStepLinear, k_screenFadeInInterpolationStep}
{
    TRACK;

    m_screenFadeIn.setTargetValue(0.f);

    auto &device = Global::getCore().getDevice();
    auto &GUIManager = device.getGUIManager();
    auto &GUIRenderer = GUIManager.getRenderer();
    const auto &screenSize = device.getScreenSize();

    int bottomShadeHeight{45};
    engine::Color bottomShadeColor{0.f, 0.f, 0.f, 0.25f};

    m_bottomShade = GUIManager.addRectWidget(engine::IntRect{0, screenSize.y - bottomShadeHeight, screenSize.x, bottomShadeHeight});
    m_bottomShade->setColors(engine::Color::k_transparentBlack, bottomShadeColor);

    addBottomRightButtons();

    m_bottomRightMessageLabel = GUIManager.addLabel(engine::IntRect{screenSize.x - 400, screenSize.y - 38, 180, 20});
    m_bottomRightMessageLabel->setCentered(false, true);
    m_bottomRightMessageLabel->setText("Press C to toggle GUI mode.");

    m_crosshair1 = GUIRenderer.getTexture(k_crosshair1Path);
    m_crosshair2 = GUIRenderer.getTexture(k_crosshair2Path);
    m_hpTexture = GUIRenderer.getTexture(k_hpTexturePath);
    m_hitTargetTexture = GUIRenderer.getTexture(k_hitTargetTexturePath);
}

void MainGUI::update()
{
    TRACK;

    updateCurrentHandyWindowAndHighlight();
    updateDeconstructionProgressBar();
    updateRevivingProgressBar();
    updateItemContainerSearchWindow();
    updateCraftingWindow();
    updateBloodSplats();
    updateCrosshairAlpha();
    m_tooltip.update();
    m_levelUpAnimation.update();
    m_screenFadeIn.update();

    if(m_inventoryWindow)
        m_inventoryWindow->update();

    if(m_skillsAndUpgradesWindow)
        m_skillsAndUpgradesWindow->update();
}

bool MainGUI::isInGUIMode() const
{
    return m_GUIMode;
}

void MainGUI::setGUIMode(bool GUIMode)
{
    TRACK;

    if(m_GUIMode != GUIMode) {
        m_GUIMode = GUIMode;

        auto &core = Global::getCore();
        auto &device = core.getDevice();

        if(m_GUIMode) {
            device.setFPPCameraControl(false);
            device.setWidgetsGetInput(true);
            m_currentHandyWindow.reset();
        }
        else {
            device.setFPPCameraControl();
            device.setWidgetsGetInput(false);

            m_inventoryWindow.reset();
            m_skillsAndUpgradesWindow.reset();
            m_itemContainerSearchWindow.reset();
            m_craftingWindow.reset();

            auto &itemContainerSearchTracker = core.getThisPlayer().getItemContainerSearchTracker();

            if(itemContainerSearchTracker.isSearchingAnything())
                itemContainerSearchTracker.stopSearching();
        }
    }
}

void MainGUI::toggleGUIMode()
{
    setGUIMode(!m_GUIMode);

    auto &core = Global::getCore();
    auto &defsCache = core.getDefsCache();
    auto &soundPool = core.getSoundPool();

    soundPool.play(defsCache.Sound_Switch1);
}

void MainGUI::toggleHandyWindow(HandyWindow::Type type)
{
    TRACK;

    auto &core = Global::getCore();
    auto &defsCache = core.getDefsCache();
    auto &soundPool = core.getSoundPool();

    if(m_currentHandyWindow && m_currentHandyWindow->getType() == type) {
        m_currentHandyWindow.reset();
        soundPool.play(defsCache.Sound_Select2);
        return;
    }

    const auto &screenSize = core.getDevice().getScreenSize();

    std::shared_ptr <engine::GUI::Button> buttonToHighlight;

    switch(type) {
    case HandyWindow::Type::Construct:
        m_currentHandyWindow = std::make_unique <HandyWindow_Construct> ();
        buttonToHighlight = m_constructButton;
        break;

    case HandyWindow::Type::SkillsAndUpgrades:
        m_currentHandyWindow = std::make_unique <HandyWindow_SkillsAndUpgrades> ();
        buttonToHighlight = m_skillsAndUpgradesButton;
        break;

    case HandyWindow::Type::Inventory:
        m_currentHandyWindow = std::make_unique <HandyWindow_Inventory> ();
        buttonToHighlight = m_inventoryButton;
        break;

    default:
        throw engine::Exception{"Handy window type not handled."};
    }

    soundPool.play(defsCache.Sound_Select1);

    if(buttonToHighlight) {
        E_DASSERT(m_highlight, "Highlight is nullptr.");
        E_DASSERT(m_highlightTexture, "Highlight texture is nullptr.");

        const auto &screenRect = buttonToHighlight->getScreenRect();
        const auto &highlightTexSize = m_highlightTexture->getSize();
        auto centerX = screenRect.pos.x + screenRect.size.x / 2;
        auto finalX = centerX - highlightTexSize.x / 2;

        m_highlight->setPosition({finalX, screenSize.y - highlightTexSize.y});
    }
}

bool MainGUI::isAnyCurrentHandyWindow() const
{
    return static_cast <bool> (m_currentHandyWindow);
}

HandyWindow &MainGUI::getCurrentHandyWindow() const
{
    if(!m_currentHandyWindow)
        throw engine::Exception{"Tried to get nullptr current handy window. This should have been checked before."};

    return *m_currentHandyWindow;
}

void MainGUI::tryAddBloodSplat()
{
    TRACK;

    if(m_bloodSplats.size() < static_cast <size_t> (k_maxSimultaneousBloodSplats))
        m_bloodSplats.emplace_back();
}

Tooltip &MainGUI::getTooltip()
{
    return m_tooltip;
}

const Tooltip &MainGUI::getTooltip() const
{
    return m_tooltip;
}

LevelUpAnimation &MainGUI::getLevelUpAnimation()
{
    return m_levelUpAnimation;
}

const LevelUpAnimation &MainGUI::getLevelUpAnimation() const
{
    return m_levelUpAnimation;
}

void MainGUI::setCrosshairVisible(bool visible)
{
    m_crosshairVisible = visible;
}

void MainGUI::openInventoryWindow()
{
    auto &inventory = Global::getCore().getThisPlayer().getCharacter().getInventory();

    m_inventoryWindow = std::make_unique <InventoryWindow> (inventory);
}

void MainGUI::openCraftingWindow(const std::shared_ptr<Structure> &optionalWorkbench)
{
    const auto &playersCharacter = Global::getCore().getThisPlayer().getCharacterPtr();

    if(optionalWorkbench)
        m_craftingWindow = std::make_unique <CraftingWindow> (playersCharacter, optionalWorkbench);
    else
        m_craftingWindow = std::make_unique <CraftingWindow> (playersCharacter);
}

void MainGUI::onHitTarget()
{
    m_hitTargetAlpha.setValueWithoutInterpolation(1.f);
    m_hitTargetAlpha.setTargetValue(0.f);
}

void MainGUI::onDraw3D()
{
    TRACK;

    auto &core = Global::getCore();

    for(const auto &elem : core.getWorld().getElectricitySystems()) {
        E_DASSERT(elem, "Electricity system is nullptr.");
        elem->draw3D();
    }

    core.getThisPlayer().getStructureCurrentlyDesignated().draw3D();
}

void MainGUI::onDraw2DBeforeWidgets()
{
    TRACK;

    for(const auto &elem : m_bloodSplats) {
        elem.draw();
    }

    drawCrosshair();

    auto &core = Global::getCore();
    const auto &thisPlayer = core.getThisPlayer();
    const auto &pointedEntity = thisPlayer.getPointedEntity();

    if(pointedEntity.isAny())
        pointedEntity.getEntity().onDraw2DInfoWhenPointed();

    if(m_currentHandyWindow)
        m_currentHandyWindow->draw();

    m_levelUpAnimation.draw();

    drawHp();
}

void MainGUI::onDraw2DAfterWidgets()
{
    TRACK;

    auto &core = Global::getCore();
    const auto &itemCurrentlyDragged = core.getThisPlayer().getItemCurrentlyDragged();

    itemCurrentlyDragged.draw();

    m_tooltip.draw();

    if(!engine::Math::fuzzyCompare(m_screenFadeIn.getCurrentValue(), 0.f)) {
        auto &device = core.getDevice();
        device.getGUIManager().getRenderer().drawFilledRect({{}, device.getScreenSize()}, {0.f, 0.f, 0.f, m_screenFadeIn.getCurrentValue()});
    }
}

void MainGUI::onInventoryButtonPressed()
{
    openInventoryWindow();
}

void MainGUI::onSkillsAndUpgradesButtonPressed()
{
    const auto &playersCharacter = Global::getCore().getThisPlayer().getCharacterPtr();

    m_skillsAndUpgradesWindow = std::make_unique <SkillsAndUpgradesWindow> (playersCharacter);
}

void MainGUI::onConstructButtonPressed()
{
    openCraftingWindow();
}

MainGUI::~MainGUI()
{
}

void MainGUI::addBottomRightButtons()
{
    TRACK;

    auto &device = Global::getCore().getDevice();
    auto &GUIManager = device.getGUIManager();
    auto &GUIRenderer = GUIManager.getRenderer();
    const auto &screenSize = device.getScreenSize();

    m_highlightTexture = GUIRenderer.getTexture(k_highlightTexturePath);
    m_highlight = GUIManager.addImage(engine::IntRect{{0, 0}, m_highlightTexture->getSize()});
    m_highlight->setTexture(m_highlightTexture);

    int bottomPadding{5};
    int rightPadding{20};
    int curX{screenSize.x - rightPadding};

    const auto &inventoryButtonTexture = GUIRenderer.getTexture(k_inventoryButtonTexturePath);
    const auto &inventoryButtonTextureSize = inventoryButtonTexture->getSize();
    m_inventoryButton = GUIManager.addButton(engine::IntRect{{curX - inventoryButtonTextureSize.x, screenSize.y - inventoryButtonTextureSize.y - bottomPadding}, inventoryButtonTextureSize});
    m_inventoryButton->setDrawBackground(false);
    m_inventoryButton->setTexture(inventoryButtonTexture);
    m_inventoryButton->setOnPressed([]() {
        Global::getCore().getMainGUI().onInventoryButtonPressed();
    });

    curX -= inventoryButtonTextureSize.x + 5;

    const auto &skillsAndUpgradesButtonTexture = GUIRenderer.getTexture(k_skillsAndUpgradesButtonTexturePath);
    const auto &skillsAndUpgradesButtonTextureSize = skillsAndUpgradesButtonTexture->getSize();
    m_skillsAndUpgradesButton = GUIManager.addButton(engine::IntRect{{curX - skillsAndUpgradesButtonTextureSize.x, screenSize.y - skillsAndUpgradesButtonTextureSize.y - bottomPadding}, skillsAndUpgradesButtonTextureSize});
    m_skillsAndUpgradesButton->setDrawBackground(false);
    m_skillsAndUpgradesButton->setTexture(skillsAndUpgradesButtonTexture);
    m_skillsAndUpgradesButton->setOnPressed([]() {
        Global::getCore().getMainGUI().onSkillsAndUpgradesButtonPressed();
    });

    curX -= skillsAndUpgradesButtonTextureSize.x + 5;

    const auto &constructButtonTexture = GUIRenderer.getTexture(k_constructButtonTexturePath);
    const auto &constructButtonTextureSize = constructButtonTexture->getSize();
    m_constructButton = GUIManager.addButton(engine::IntRect{{curX - constructButtonTextureSize.x, screenSize.y - constructButtonTextureSize.y - bottomPadding}, constructButtonTextureSize});
    m_constructButton->setDrawBackground(false);
    m_constructButton->setTexture(constructButtonTexture);
    m_constructButton->setOnPressed([]() {
        Global::getCore().getMainGUI().onConstructButtonPressed();
    });

    curX -= inventoryButtonTextureSize.x + 5;
}

void MainGUI::updateCurrentHandyWindowAndHighlight()
{
    if(m_currentHandyWindow) {
        m_highlightAlpha.setTargetValue(1.f);
        m_currentHandyWindow->update();
    }
    else
        m_highlightAlpha.setTargetValue(0.f);

    m_highlightAlpha.update();

    E_DASSERT(m_highlight, "Highlight is nullptr.");

    m_highlight->setColor({1.f, 1.f, 1.f, m_highlightAlpha.getCurrentValue()});
}

void MainGUI::updateDeconstructionProgressBar()
{
    auto &core = Global::getCore();
    const auto &thisPlayer = core.getThisPlayer();
    const auto &deconstructionTracker = thisPlayer.getDeconstructionTracker();

    if(deconstructionTracker.isDeconstructingAnything()) {
        if(!m_deconstructionProgressBar) {
            auto &device = core.getDevice();
            auto &GUIManager = device.getGUIManager();
            const auto &screenSize = device.getScreenSize();

            m_deconstructionProgressBar = GUIManager.addProgressBar({(screenSize - k_deconstructionProgressBarSize) / 2, k_deconstructionProgressBarSize});
        }

        m_deconstructionProgressBar->setProgress(deconstructionTracker.getDeconstructionProgress());
    }
    else
        m_deconstructionProgressBar.reset();
}

void MainGUI::updateRevivingProgressBar()
{
    auto &core = Global::getCore();
    const auto &thisPlayer = core.getThisPlayer();
    const auto &revivingTracker = thisPlayer.getRevivingTracker();

    if(revivingTracker.isRevivingAnything()) {
        if(!m_revivingProgressBar) {
            auto &device = core.getDevice();
            auto &GUIManager = device.getGUIManager();
            const auto &screenSize = device.getScreenSize();

            m_revivingProgressBar = GUIManager.addProgressBar({(screenSize - k_revivingProgressBarSize) / 2, k_revivingProgressBarSize});
        }

        m_revivingProgressBar->setProgress(revivingTracker.getRevivingProgress());
    }
    else
        m_revivingProgressBar.reset();
}

void MainGUI::updateItemContainerSearchWindow()
{
    auto &itemContainerSearchTracker = Global::getCore().getThisPlayer().getItemContainerSearchTracker();

    if(!itemContainerSearchTracker.isSearchingAnything()) {
        m_itemContainerSearchWindow.reset();
        return;
    }

    auto &searchedEntity = itemContainerSearchTracker.getSearchedEntity();
    auto searchedItemContainer = searchedEntity.getSearchableItemContainer();

    if(!m_itemContainerSearchWindow) {
        if(m_inventoryWindow)
            m_itemContainerSearchWindow = std::make_unique <ItemContainerWindow> (searchedItemContainer, m_inventoryWindow->getWindow());
        else
            m_itemContainerSearchWindow = std::make_unique <ItemContainerWindow> (searchedItemContainer);
    }
    else if(m_itemContainerSearchWindow->isClosed()) { // user closed search window
        m_itemContainerSearchWindow.reset();
        itemContainerSearchTracker.stopSearching();
        return;
    }

    m_itemContainerSearchWindow->update(searchedEntity.getName(), searchedItemContainer);
}

void MainGUI::updateCraftingWindow()
{
    if(!m_craftingWindow || m_craftingWindow->isClosed())
        return;

    if(m_craftingWindow->hasWorkbench()) {
        const auto &workbench = m_craftingWindow->getWorkbench();

        if(!workbench.isInWorld()) {
            m_craftingWindow.reset();
            return;
        }

        float distSq{workbench.getInWorldPosition().getDistanceSq(m_craftingWindow->getCharacter().getInWorldPosition())};

        if(distSq > CraftingWindow::k_maxCharacterDistanceToWorkbench * CraftingWindow::k_maxCharacterDistanceToWorkbench) {
            m_craftingWindow.reset();
            return;
        }
    }

    m_craftingWindow->update();
}

void MainGUI::updateBloodSplats()
{
    for(auto &elem : m_bloodSplats) {
        elem.update();
    }

    for(size_t i = 0; i < m_bloodSplats.size(); ++i) {
        if(m_bloodSplats[i].wantsToBeRemoved()) {
            std::swap(m_bloodSplats[i], m_bloodSplats.back());
            m_bloodSplats.pop_back();
        }
    }
}

void MainGUI::updateCrosshairAlpha()
{
    const auto &projectilesSpreadAngleManager = Global::getCore().getThisPlayer().getCharacter().getProjectilesSpreadAngleManager();

    if(m_crosshairVisible && !projectilesSpreadAngleManager.isAiming())
        m_crosshairAlpha.setTargetValue(1.f);
    else
        m_crosshairAlpha.setTargetValue(0.f);

    m_crosshairAlpha.update();
    m_hitTargetAlpha.update();
}

void MainGUI::drawCrosshair()
{
    E_DASSERT(m_crosshair1, "Crosshair 1 is nullptr.");
    E_DASSERT(m_crosshair2, "Crosshair 2 is nullptr.");
    E_DASSERT(m_hitTargetTexture, "Hit target texture is nullptr.");

    auto &core = Global::getCore();
    auto &device = core.getDevice();
    const auto &screenSize = device.getScreenSize();
    const auto &projectilesSpreadAngleManager = core.getThisPlayer().getCharacter().getProjectilesSpreadAngleManager();
    const auto &center = screenSize / 2;

    engine::Color hitTargetTextureColor{1.f, 1.f, 1.f, m_hitTargetAlpha.getCurrentValue()};
    m_hitTargetTexture->draw(center - m_hitTargetTexture->getSize() / 2, hitTargetTextureColor);

    float FOV{device.getSceneManager().getFOV()};
    float angle{projectilesSpreadAngleManager.getCurrentSpreadAngle()};
    float distance{std::tan(engine::Math::degToRad(angle)) / std::tan(FOV / 2.f) * screenSize.x / 2.f};

    if(distance < 1.f)
        distance = 1.f;

    const auto &crosshair1Size = m_crosshair1->getSize();
    const auto &crosshair2Size = m_crosshair2->getSize();

    engine::IntVec2 down{center.x - crosshair1Size.x / 2,
                         static_cast <int> (center.y + distance)};

    engine::IntVec2 up{center.x - crosshair1Size.x / 2,
                       static_cast <int> (center.y - distance - crosshair1Size.y)};

    engine::IntVec2 left{static_cast <int> (center.x - distance - crosshair2Size.x),
                         center.y - crosshair2Size.y / 2};

    engine::IntVec2 right{static_cast <int> (center.x + distance),
                          center.y - crosshair2Size.y / 2};

    engine::Color color{1.f, 1.f, 1.f, m_crosshairAlpha.getCurrentValue()};

    m_crosshair1->draw(up, color);
    m_crosshair1->draw(down, color);
    m_crosshair2->draw(left, color);
    m_crosshair2->draw(right, color);
}

void MainGUI::drawHp()
{
    auto &core = Global::getCore();
    auto &device = core.getDevice();
    const auto &screenSize = device.getScreenSize();

    E_DASSERT(m_hpTexture, "Hp texture is nullptr.");

    const auto &size = m_hpTexture->getSize();
    engine::IntVec2 pos{5, screenSize.y - 5 - size.y};

    m_hpTexture->draw(pos, {1.f, 0.5f, 0.5f});

    auto &renderer = device.getGUIManager().getRenderer();
    auto hp = core.getThisPlayer().getCharacter().getHP();

    renderer.drawText(std::to_string(hp), {pos.movedX(size.x + 10), {500, size.y}}, engine::Color::k_white, engine::GUI::IGUIRenderer::FontSize::Medium, engine::GUI::IGUIRenderer::HorizontalTextAlign::Left, engine::GUI::IGUIRenderer::VerticalTextAlign::Middle);
}

const std::string MainGUI::k_crosshair1Path = "GUI/crosshair1.png";
const std::string MainGUI::k_crosshair2Path = "GUI/crosshair2.png";
const std::string MainGUI::k_highlightTexturePath = "GUI/highlight.png";
const std::string MainGUI::k_inventoryButtonTexturePath = "GUI/icons/inventory.png";
const std::string MainGUI::k_skillsAndUpgradesButtonTexturePath = "GUI/icons/skillsAndUpgrades.png";
const std::string MainGUI::k_constructButtonTexturePath = "GUI/icons/construct.png";
const std::string MainGUI::k_hpTexturePath = "GUI/hp.png";
const std::string MainGUI::k_hitTargetTexturePath = "GUI/hitTarget.png";
const float MainGUI::k_highlightAlphaInterpolationStep{5.f};
const float MainGUI::k_crosshairAlphaInterpolationStep{7.f};
const float MainGUI::k_hitTargetAlphaInterpolationStep{5.f};
const float MainGUI::k_screenFadeInInterpolationStep{1.f};
const engine::IntVec2 MainGUI::k_deconstructionProgressBarSize{300, 20};
const engine::IntVec2 MainGUI::k_revivingProgressBarSize{300, 20};
const int MainGUI::k_maxSimultaneousBloodSplats{5};

} // namespace app
