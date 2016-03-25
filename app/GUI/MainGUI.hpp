#ifndef APP_MAIN_GUI_HPP
#define APP_MAIN_GUI_HPP

#include "immediate/HandyWindow.hpp"
#include "immediate/BloodSplat.hpp"
#include "immediate/Tooltip.hpp"
#include "immediate/LevelUpAnimation.hpp"
#include "../util/InterpolatedFloat.hpp"

#include <memory>
#include <vector>

namespace engine
{
namespace GUI
{

class IGUITexture;
class RectWidget;
class Button;
class Label;
class Image;
class ProgressBar;

} // namespace GUI
} // namespace engine

namespace app
{

class InventoryWindow;
class SkillsAndUpgradesWindow;
class ItemContainerWindow;
class CraftingWindow;
class Structure;

class MainGUI
{
public:
    MainGUI();

    void update();

    bool isInGUIMode() const;
    void setGUIMode(bool GUIMode);
    void toggleGUIMode();

    void toggleHandyWindow(HandyWindow::Type type);
    bool isAnyCurrentHandyWindow() const;
    HandyWindow &getCurrentHandyWindow() const;

    void tryAddBloodSplat();

    Tooltip &getTooltip();
    const Tooltip &getTooltip() const;

    LevelUpAnimation &getLevelUpAnimation();
    const LevelUpAnimation &getLevelUpAnimation() const;

    void setCrosshairVisible(bool visible);
    void openInventoryWindow();
    void openCraftingWindow(const std::shared_ptr <Structure> &optionalWorkbench = {});

    void onHitTarget();
    void onDraw3D();
    void onDraw2DBeforeWidgets();
    void onDraw2DAfterWidgets();
    void onInventoryButtonPressed();
    void onSkillsAndUpgradesButtonPressed();
    void onConstructButtonPressed();

    ~MainGUI();

private:
    void addBottomRightButtons();

    void updateCurrentHandyWindowAndHighlight();
    void updateDeconstructionProgressBar();
    void updateRevivingProgressBar();
    void updateItemContainerSearchWindow();
    void updateCraftingWindow();
    void updateBloodSplats();
    void updateCrosshairAlpha();

    void drawCrosshair();
    void drawHp();

    static const std::string k_crosshair1Path;
    static const std::string k_crosshair2Path;
    static const std::string k_highlightTexturePath;
    static const std::string k_inventoryButtonTexturePath;
    static const std::string k_skillsAndUpgradesButtonTexturePath;
    static const std::string k_constructButtonTexturePath;
    static const std::string k_hpTexturePath;
    static const std::string k_hitTargetTexturePath;
    static const float k_highlightAlphaInterpolationStep;
    static const float k_crosshairAlphaInterpolationStep;
    static const float k_hitTargetAlphaInterpolationStep;
    static const float k_screenFadeInInterpolationStep;
    static const engine::IntVec2 k_deconstructionProgressBarSize;
    static const engine::IntVec2 k_revivingProgressBarSize;
    static const int k_maxSimultaneousBloodSplats;

    bool m_GUIMode;
    bool m_crosshairVisible;
    InterpolatedFloat m_highlightAlpha;
    InterpolatedFloat m_crosshairAlpha;
    InterpolatedFloat m_hitTargetAlpha;
    InterpolatedFloat m_screenFadeIn;
    LevelUpAnimation m_levelUpAnimation;
    Tooltip m_tooltip;

    // widgets

    std::shared_ptr <engine::GUI::RectWidget> m_bottomShade;
    std::shared_ptr <engine::GUI::Label> m_bottomRightMessageLabel;
    std::shared_ptr <engine::GUI::Image> m_highlight;
    std::shared_ptr <engine::GUI::Button> m_inventoryButton;
    std::shared_ptr <engine::GUI::Button> m_skillsAndUpgradesButton;
    std::shared_ptr <engine::GUI::Button> m_constructButton;
    std::shared_ptr <engine::GUI::ProgressBar> m_deconstructionProgressBar;
    std::shared_ptr <engine::GUI::ProgressBar> m_revivingProgressBar;

    // widget packs

    std::unique_ptr <InventoryWindow> m_inventoryWindow;
    std::unique_ptr <SkillsAndUpgradesWindow> m_skillsAndUpgradesWindow;
    std::unique_ptr <ItemContainerWindow> m_itemContainerSearchWindow;
    std::unique_ptr <CraftingWindow> m_craftingWindow;

    // handy window

    std::unique_ptr <HandyWindow> m_currentHandyWindow;

    // textures

    std::shared_ptr <engine::GUI::IGUITexture> m_crosshair1;
    std::shared_ptr <engine::GUI::IGUITexture> m_crosshair2;
    std::shared_ptr <engine::GUI::IGUITexture> m_highlightTexture;
    std::shared_ptr <engine::GUI::IGUITexture> m_hpTexture;
    std::shared_ptr <engine::GUI::IGUITexture> m_hitTargetTexture;

    // blood splats

    std::vector <BloodSplat> m_bloodSplats;
};

} // namespace app

#endif // APP_MAIN_GUI_HPP
