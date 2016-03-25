#ifndef APP_FPP_ITEM_MODEL_CONTROLLER_HPP
#define APP_FPP_ITEM_MODEL_CONTROLLER_HPP

#include "engine/util/Vec3.hpp"
#include "../util/InterpolatedFloat.hpp"
#include "../util/InterpolatedFloatVec3.hpp"
#include "../util/InterpolatedLoopedFloatVec3.hpp"

#include <memory>

namespace engine { namespace app3D { class Model; } }

namespace app
{

class Item;
class ItemDef;

class FPPItemModelController
{
public:
    enum class CurrentAction
    {
        None,
        SwitchingItem,
        Using,
        Reloading
    };

    FPPItemModelController();

    void update(const std::shared_ptr <Item> &currentItem, float playerMovementTimeAccumulator);

    bool setAiming(bool isAiming);
    bool isAiming() const;
    bool toggleAiming();

    void use();
    bool reload();

    void onFinishedUsingReloadingOrEquipping();
    void onPreviousItemPutAway();

    CurrentAction getCurrentAction() const;
    bool hasItem() const;

private:
    void currentItemChanged(const std::shared_ptr <Item> &currentItem);
    void updateInterpolation();
    void updateCameraMovementItemRotOffset();
    void updatePlayerMovementItemPosOffset(float playerMovementTimeAccumulator);
    void updateCurrentModelPosAndRot();

    static const float k_posInterpolationDuration;
    static const float k_rotInterpolationDuration;
    static const float k_FOVMultiplierInterpolationDuration;
    static const float k_FOVMultiplierWhileAiming;
    static const float k_cameraMovementItemRotOffsetAccumulatorInterpolationStep;
    static const float k_cameraMovementItemRotOffsetInterpolationStep;
    static const float k_playerMovementItemPosOffsetInterpolationStep;
    static const float k_cameraMovementItemRotOffsetPerCameraMovementMultiplier;
    static const float k_maxCameraMovementItemRotOffset;
    static const float k_maxPlayerMovementItemPosOffsetHorizontalDeviation;
    static const float k_maxPlayerMovementItemPosOffsetVerticalDeviation;

    int m_currentItemEntityID;
    bool m_isAiming;
    CurrentAction m_currentAction;
    std::shared_ptr <ItemDef> m_currentItemDef;
    std::shared_ptr <engine::app3D::Model> m_currentModel;
    engine::FloatVec3 m_prevCameraRot;
    InterpolatedFloat m_FOVMultiplier;
    InterpolatedFloatVec3 m_pos;
    InterpolatedLoopedFloatVec3 m_rot;
    InterpolatedLoopedFloatVec3 m_cameraMovementItemRotOffsetAccumulator;
    InterpolatedLoopedFloatVec3 m_cameraMovementItemRotOffset;
    InterpolatedFloatVec3 m_playerMovementItemPosOffset;
    int m_previouslyUsedAnimationIndex;
};

} // namespace app

#endif // APP_FPP_ITEM_MODEL_CONTROLLER_HPP
