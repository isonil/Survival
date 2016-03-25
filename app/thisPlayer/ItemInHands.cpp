#include "ItemInHands.hpp"

#include "engine/app3D/managers/SceneManager.hpp"
#include "engine/app3D/Device.hpp"
#include "../GUI/MainGUI.hpp"
#include "../defs/ItemDef.hpp"
#include "../entities/Character.hpp"
#include "../entities/Item.hpp"
#include "../itemContainers/SingleSlotItemContainer.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "ThisPlayer.hpp"

namespace app
{

ItemInHands::ItemInHands()
    : m_continuousUse{}
{
}

void ItemInHands::update()
{
    TRACK;

    auto &core = Global::getCore();
    auto &thisPlayer = core.getThisPlayer();
    auto &character = thisPlayer.getCharacter();

    for(size_t i = 0; i < m_onUsedTimers.size();) {
        if(m_onUsedTimers[i].passed()) {
            const auto &lookVec = core.getDevice().getSceneManager().getCameraLookVec();
            character.tryUseItemInHands(lookVec);

            std::swap(m_onUsedTimers[i], m_onUsedTimers.back());
            m_onUsedTimers.pop_back();
        }
        else
            ++i;
    }

    if(m_continuousUse) {
        if(!canBeUsedContinuously())
            m_continuousUse = false;
        else
            tryUse();
    }

    auto &FPPItemModelController = thisPlayer.getFPPItemModelController();
    auto &structureCurrentlyDesignated = thisPlayer.getStructureCurrentlyDesignated();
    auto &mainGUI = core.getMainGUI();

    if(structureCurrentlyDesignated.hasStructure() ||
       (FPPItemModelController.getCurrentAction() != FPPItemModelController::CurrentAction::None &&
        FPPItemModelController.getCurrentAction() != FPPItemModelController::CurrentAction::Using))
        mainGUI.setCrosshairVisible(false);
    else
        mainGUI.setCrosshairVisible(true);
}

bool ItemInHands::hasItem() const
{
    return Global::getCore().getThisPlayer().getCharacter().getInventory().getHandsItemContainer().hasItem();
}

Item &ItemInHands::getItem() const
{
    if(!hasItem())
        throw engine::Exception{"Tried to get nullptr item. This should have been checked before."};

    return Global::getCore().getThisPlayer().getCharacter().getInventory().getHandsItemContainer().getItem();
}

void ItemInHands::tryUse()
{
    TRACK;

    auto &thisPlayer = Global::getCore().getThisPlayer();
    auto &character = thisPlayer.getCharacter();
    auto &handsItemContainer = character.getInventory().getHandsItemContainer();
    const auto &structureCurrentlyDesignated = thisPlayer.getStructureCurrentlyDesignated();
    auto &FPPItemModelController = thisPlayer.getFPPItemModelController();

    if(!handsItemContainer.hasItem() || structureCurrentlyDesignated.hasStructure())
        return;

    // even though we have something in our 'hands slot', we can still be underwater with our FPP weapon hidden
    if(!FPPItemModelController.hasItem())
        return;

    if(FPPItemModelController.getCurrentAction() != FPPItemModelController::CurrentAction::None)
        return;

    FPPItemModelController.use();

    //float timeBetween{handsItemContainer.getItem().getDef().getOnUsed().getTimeBetweenUseAndActualEffects()};
    // TODO
    float timeBetween{};

    m_onUsedTimers.push_back(Timer(timeBetween));
}

void ItemInHands::tryStartContinuousUse()
{
    if(canBeUsedContinuously())
        m_continuousUse = true;
}

void ItemInHands::tryStopContinuousUse()
{
    m_continuousUse = false;
}

bool ItemInHands::canBeUsedContinuously() const
{
    auto &thisPlayer = Global::getCore().getThisPlayer();
    auto &FPPItemModelController = thisPlayer.getFPPItemModelController();

    if(!FPPItemModelController.hasItem())
        return false;

    auto &handsItemContainer = thisPlayer.getCharacter().getInventory().getHandsItemContainer();
    return handsItemContainer.hasItem() && handsItemContainer.getItem().getDef().getOnUsed().isContinuousUse();
}

} // namespace app
