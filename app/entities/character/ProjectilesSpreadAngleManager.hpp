#ifndef APP_PROJECTILES_SPREAD_ANGLE_MANAGER_HPP
#define APP_PROJECTILES_SPREAD_ANGLE_MANAGER_HPP

#include "../../util/InterpolatedFloat.hpp"

#include <memory>

namespace app
{

class Character;
class SingleSlotItemContainer;

class ProjectilesSpreadAngleManager
{
public:
    ProjectilesSpreadAngleManager(const Character &character, const std::shared_ptr <SingleSlotItemContainer> &handsItemContainer);

    void update();

    void onUsedItem();

    void setAiming(bool aiming);
    bool isAiming() const;

    float getCurrentSpreadAngle() const;

private:
    static const float k_spreadAngleInterpolationStep;
    static const float k_spreadAngleDecreaseInterpolationStep;
    static const float k_maxProjectilesSpreadAngleMultiplier;
    static const float k_spreadAngleOffsetDueToMovementMultiplier;
    static const float k_aimingSpreadAngle;

    const Character &m_character;
    std::shared_ptr <SingleSlotItemContainer> m_handsItemContainer;
    InterpolatedFloat m_spreadAngle; // quickly interpolates towards m_spreadAngleDecrease
    InterpolatedFloat m_spreadAngleDecrease; // slowly interpolates towards min spread angle
    bool m_aiming;
};

} // namespace app

#endif // APP_PROJECTILES_SPREAD_ANGLE_MANAGER_HPP
