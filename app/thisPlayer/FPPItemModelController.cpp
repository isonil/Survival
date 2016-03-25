#include "FPPItemModelController.hpp"

#include "../entities/character/ProjectilesSpreadAngleManager.hpp"
#include "engine/app3D/managers/SceneManager.hpp"
#include "engine/app3D/sceneNodes/Model.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/util/Range.hpp"
#include "../defs/ItemDef.hpp"
#include "../defs/AnimationFramesSetDef.hpp"
#include "../entities/Item.hpp"
#include "../entities/Character.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "../SoundPool.hpp"
#include "ThisPlayer.hpp"

namespace app
{

FPPItemModelController::FPPItemModelController()
    : m_currentItemEntityID{-1},
      m_isAiming{},
      m_currentAction{CurrentAction::None},
      m_prevCameraRot{Global::getCore().getDevice().getSceneManager().getCameraRotation()},
      m_FOVMultiplier{1.f, InterpolationType::FixedDuration, k_FOVMultiplierInterpolationDuration},
      m_pos{{}, InterpolationType::FixedDuration, k_posInterpolationDuration},
      m_rot{{}, InterpolationType::FixedDuration, k_rotInterpolationDuration, {0.f, 360.f}},
      m_cameraMovementItemRotOffsetAccumulator{{}, InterpolationType::FixedStepLinear, k_cameraMovementItemRotOffsetAccumulatorInterpolationStep, {0.f, 360.f}},
      m_cameraMovementItemRotOffset{{}, InterpolationType::FixedStepSmooth, k_cameraMovementItemRotOffsetInterpolationStep, {0.f, 360.f}, 1.5f},
      m_playerMovementItemPosOffset{{}, InterpolationType::FixedStepLinear, k_playerMovementItemPosOffsetInterpolationStep},
      m_previouslyUsedAnimationIndex{-1}
{
}

void FPPItemModelController::update(const std::shared_ptr <Item> &currentItem, float playerMovementTimeAccumulator)
{
    TRACK;

    if(m_currentItemEntityID >= 0) {
        if(!currentItem || currentItem->getEntityID() != m_currentItemEntityID)
            currentItemChanged(currentItem);
    }
    else if(currentItem)
        currentItemChanged(currentItem);

    updateInterpolation();
    updateCameraMovementItemRotOffset();
    updatePlayerMovementItemPosOffset(playerMovementTimeAccumulator);
    updateCurrentModelPosAndRot();

    auto &core = Global::getCore();
    auto &sceneManager = core.getDevice().getSceneManager();
    sceneManager.setFOVMultiplier(m_FOVMultiplier.getCurrentValue());

    // update projectiles spread angle manager isAiming

    auto &projectilesSpreadAngleManager = core.getThisPlayer().getCharacter().getProjectilesSpreadAngleManager();

    if(isAiming())
        projectilesSpreadAngleManager.setAiming(true);
    else
        projectilesSpreadAngleManager.setAiming(false);
}

bool FPPItemModelController::setAiming(bool isAiming)
{
    TRACK;

    if(isAiming != m_isAiming) {
        if(isAiming) {
            if(!m_currentItemDef || !m_currentModel)
                return false;

            if(m_currentAction != CurrentAction::None &&
               m_currentAction != CurrentAction::Using)
                return false;
        }

        m_isAiming = isAiming;

        if(!m_currentItemDef)
            return false;

        const auto &FPPProperties = m_currentItemDef->getFPPProperties();

        if(m_isAiming) {
            m_pos.setTargetVec(FPPProperties.getAimPosition());
            m_rot.setTargetVec(FPPProperties.getAimRotation());
            m_FOVMultiplier.setTargetValue(k_FOVMultiplierWhileAiming);
        }
        else {
            m_pos.setTargetVec(FPPProperties.getBasePosition());
            m_rot.setTargetVec(FPPProperties.getBaseRotation());
            m_FOVMultiplier.setTargetValue(1.f);
        }

        return true;
    }

    return false;
}

bool FPPItemModelController::isAiming() const
{
    return m_isAiming;
}

bool FPPItemModelController::toggleAiming()
{
    return setAiming(!m_isAiming);
}

void FPPItemModelController::use()
{
    TRACK;

    if(!m_currentItemDef || !m_currentModel)
        return;

    if(m_currentAction != CurrentAction::None)
        return;

    m_currentAction = CurrentAction::Using;

    const auto &FPPProperties = m_currentItemDef->getFPPProperties();

    if(FPPProperties.hasAnimationFramesSetDef()) {
        const auto &animationFramesSetDef = FPPProperties.getAnimationFramesSetDef();
        const auto &onUsed = m_currentItemDef->getOnUsed();

        int index{onUsed.getRandomUseAnimationIndex(m_previouslyUsedAnimationIndex)};
        m_previouslyUsedAnimationIndex = index;

        const auto &useAnimation = onUsed.getUseAnimations()[index];
        const auto &FPPAnimation = animationFramesSetDef.getUse_FPP_byIndex(useAnimation.getFPPAnimationIndex());

        m_currentModel->playSingleAnimation(FPPAnimation, []() {
            Global::getCore().getThisPlayer().getFPPItemModelController().onFinishedUsingReloadingOrEquipping();
        });
    }
    else
        onFinishedUsingReloadingOrEquipping();
}

bool FPPItemModelController::reload()
{
    TRACK;

    if(!m_currentItemDef || !m_currentModel)
        return false;

    if(m_currentAction != CurrentAction::None)
        return false;

    setAiming(false);

    m_currentAction = CurrentAction::Reloading;

    const auto &FPPProperties = m_currentItemDef->getFPPProperties();

    if(FPPProperties.hasAnimationFramesSetDef()) {
        const auto &animationFramesSetDef = FPPProperties.getAnimationFramesSetDef();

        m_currentModel->playSingleAnimation(animationFramesSetDef.getReload_FPP(), []() {
            Global::getCore().getThisPlayer().getFPPItemModelController().onFinishedUsingReloadingOrEquipping();
        });
    }
    else
        onFinishedUsingReloadingOrEquipping();

    return true;
}

void FPPItemModelController::currentItemChanged(const std::shared_ptr <Item> &currentItem)
{
    TRACK;

    setAiming(false);

    m_currentAction = CurrentAction::SwitchingItem;

    engine::IntRange putAwayAnimationFrames;

    auto &core = Global::getCore();
    auto &thisPlayer = core.getThisPlayer();

    if(m_currentItemDef) {
        const auto &FPPProperties = m_currentItemDef->getFPPProperties();

        if(FPPProperties.hasAnimationFramesSetDef())
            putAwayAnimationFrames = FPPProperties.getAnimationFramesSetDef().getPutAway_FPP();

        if(m_currentItemDef->hasPutAwaySoundDef())
            core.getSoundPool().play(m_currentItemDef->getPutAwaySoundDefPtr(), thisPlayer.getCharacterPtr());
    }

    if(currentItem) {
        m_currentItemEntityID = currentItem->getEntityID();
        m_currentItemDef = currentItem->getDefPtr();
    }
    else {
        m_currentItemEntityID = -1;
        m_currentItemDef.reset();
    }

    if(m_currentModel) {
        if(!putAwayAnimationFrames.isEmpty()) {
            m_currentModel->playSingleAnimation(putAwayAnimationFrames, []() {
                Global::getCore().getThisPlayer().getFPPItemModelController().onPreviousItemPutAway();
            });
        }
        else
            onPreviousItemPutAway();
    }
    else
        onPreviousItemPutAway();
}

void FPPItemModelController::onFinishedUsingReloadingOrEquipping()
{
    TRACK;

    m_currentAction = CurrentAction::None;

    if(!m_currentItemDef || !m_currentModel) // shouldn't ever happen
        return;

    const auto &FPPProperties = m_currentItemDef->getFPPProperties();

    if(FPPProperties.hasAnimationFramesSetDef()) {
        const auto &animationFramesSetDef = FPPProperties.getAnimationFramesSetDef();
        m_currentModel->playAnimationLoop(animationFramesSetDef.getIdle_FPP());
    }
}

void FPPItemModelController::onPreviousItemPutAway()
{
    TRACK;

    // we assume that at this point previous model was moved completely off the screen
    // so we can just remove it

    m_currentModel.reset();

    auto &core = Global::getCore();
    auto &thisPlayer = core.getThisPlayer();

    if(m_currentItemDef && m_currentItemDef->hasEquipSoundDef())
        core.getSoundPool().play(m_currentItemDef->getEquipSoundDefPtr(), thisPlayer.getCharacterPtr());

    // previous item was put away, so now we have to equip new one (if there is one)

    if(m_currentItemDef && m_currentItemDef->getFPPProperties().hasModel()) {
        const auto &FPPProperties = m_currentItemDef->getFPPProperties();

        if(m_isAiming) {
            m_pos.setVecWithoutInterpolation(FPPProperties.getAimPosition());
            m_rot.setVecWithoutInterpolation(FPPProperties.getAimRotation());
        }
        else {
            m_pos.setVecWithoutInterpolation(FPPProperties.getBasePosition());
            m_rot.setVecWithoutInterpolation(FPPProperties.getBaseRotation());
        }

        auto &sceneManager = core.getDevice().getSceneManager();
        m_currentModel = sceneManager.addModel(FPPProperties.getModelDefPtr(), true);

        if(FPPProperties.hasAnimationFramesSetDef()) {
            const auto &animationFramesSetDef = FPPProperties.getAnimationFramesSetDef();
            const auto &equipAnimationFrames = animationFramesSetDef.getEquip_FPP();

            m_currentModel->playSingleAnimation(equipAnimationFrames, []() {
                Global::getCore().getThisPlayer().getFPPItemModelController().onFinishedUsingReloadingOrEquipping();
            });
        }
        else
            onFinishedUsingReloadingOrEquipping();

        updateCurrentModelPosAndRot();
    }
    else
        m_currentAction = CurrentAction::None;

    m_previouslyUsedAnimationIndex = -1;
}

FPPItemModelController::CurrentAction FPPItemModelController::getCurrentAction() const
{
    return m_currentAction;
}

bool FPPItemModelController::hasItem() const
{
    return static_cast <bool> (m_currentItemDef);
}

void FPPItemModelController::updateInterpolation()
{
    m_FOVMultiplier.update();
    m_pos.update();
    m_rot.update();
    m_cameraMovementItemRotOffsetAccumulator.update();
    m_cameraMovementItemRotOffset.update();
    m_playerMovementItemPosOffset.update();
}

void FPPItemModelController::updateCameraMovementItemRotOffset()
{
    const auto &sceneManager = Global::getCore().getDevice().getSceneManager();
    const auto &currentCameraRot = sceneManager.getCameraRotation();

    float cameraDiffVertical{engine::Math::getLoopedDistanceWithSign(currentCameraRot.x, m_prevCameraRot.x, {0.f, 360.f})};
    float cameraDiffHorizontal{engine::Math::getLoopedDistanceWithSign(currentCameraRot.y, m_prevCameraRot.y, {0.f, 360.f})};

    auto vec = m_cameraMovementItemRotOffsetAccumulator.getCurrentVec();

    vec.y -= cameraDiffHorizontal * k_cameraMovementItemRotOffsetPerCameraMovementMultiplier;
    vec.x += cameraDiffVertical * k_cameraMovementItemRotOffsetPerCameraMovementMultiplier;

    float dist{vec.getLoopedDistance({}, {0.f, 360.f})};

    if(dist > k_maxCameraMovementItemRotOffset)
        vec.moveLoopedCloserToBy({}, dist - k_maxCameraMovementItemRotOffset, {0.f, 360.f});

    m_cameraMovementItemRotOffsetAccumulator.setVecWithoutInterpolation(vec);
    m_cameraMovementItemRotOffsetAccumulator.setTargetVec({});

    m_cameraMovementItemRotOffset.setTargetVec(vec);

    m_prevCameraRot = currentCameraRot;
}

void FPPItemModelController::updatePlayerMovementItemPosOffset(float playerMovementTimeAccumulator)
{
    TRACK;

    if(engine::Math::fuzzyCompare(playerMovementTimeAccumulator, 0.f)) {
        m_playerMovementItemPosOffset.setTargetVec({});
        return;
    }

    engine::FloatVec3 vec;
    float t{std::fmod(playerMovementTimeAccumulator, 1.f)};

    auto x = static_cast <float> (sin(1.5f * engine::Math::k_pi + t * 2.f * engine::Math::k_pi));
    vec.x = x * k_maxPlayerMovementItemPosOffsetHorizontalDeviation;

    float tForY{std::fmod(t, 0.5f) * 2.f};

    float y{};

    if(tForY < 0.5f)
        y = -tForY * 2.f;
    else
        y = -1.f + sin((tForY - 0.5f) * engine::Math::k_pi);

    vec.y = y * k_maxPlayerMovementItemPosOffsetVerticalDeviation;

    m_playerMovementItemPosOffset.setTargetVec(vec);
}

void FPPItemModelController::updateCurrentModelPosAndRot()
{
    if(m_currentModel) {
        m_currentModel->setPosition(m_pos.getCurrentVec() + m_playerMovementItemPosOffset.getCurrentVec());
        m_currentModel->setRotation(m_rot.getCurrentVec() + m_cameraMovementItemRotOffset.getCurrentVec());
    }
}

const float FPPItemModelController::k_posInterpolationDuration{180.0};
const float FPPItemModelController::k_rotInterpolationDuration{180.0};
const float FPPItemModelController::k_FOVMultiplierInterpolationDuration{180.0};
const float FPPItemModelController::k_FOVMultiplierWhileAiming{0.75f};
const float FPPItemModelController::k_cameraMovementItemRotOffsetAccumulatorInterpolationStep{15.f};
const float FPPItemModelController::k_cameraMovementItemRotOffsetInterpolationStep{25.f};
const float FPPItemModelController::k_playerMovementItemPosOffsetInterpolationStep{0.05f};
const float FPPItemModelController::k_cameraMovementItemRotOffsetPerCameraMovementMultiplier{0.05f};
const float FPPItemModelController::k_maxCameraMovementItemRotOffset{7.f};
const float FPPItemModelController::k_maxPlayerMovementItemPosOffsetHorizontalDeviation{0.01f};
const float FPPItemModelController::k_maxPlayerMovementItemPosOffsetVerticalDeviation{0.015f};

} // namespace app
