#include "ProjectilesSpreadAngleManager.hpp"

#include "../../itemContainers/SingleSlotItemContainer.hpp"
#include "../../Global.hpp"
#include "../../Core.hpp"
#include "../Character.hpp"
#include "CharacterStatsOrSkillsRelatedFormulas.hpp"

namespace app
{

ProjectilesSpreadAngleManager::ProjectilesSpreadAngleManager(const Character &character, const std::shared_ptr <SingleSlotItemContainer> &handsItemContainer)
    : m_character{character},
      m_handsItemContainer{handsItemContainer},
      m_spreadAngle{0.f, InterpolationType::FixedStepLinear, k_spreadAngleInterpolationStep},
      m_spreadAngleDecrease{0.f, InterpolationType::FixedStepLinear, k_spreadAngleDecreaseInterpolationStep},
      m_aiming{}
{
    if(!m_handsItemContainer)
        throw engine::Exception{"Hands item container is nullptr."};
}

void ProjectilesSpreadAngleManager::update()
{
    m_spreadAngle.update();
    m_spreadAngleDecrease.update();

    E_DASSERT(m_handsItemContainer, "Hands item container is nullptr.");

    if(m_aiming)
        m_spreadAngleDecrease.setTargetValue(k_aimingSpreadAngle);
    else {
        float minProjectilesSpreadAngle{};

        if(m_handsItemContainer->hasItem())
            minProjectilesSpreadAngle = CharacterStatsOrSkillsRelatedFormulas::getMinProjectilesSpreadAngle(m_character, m_handsItemContainer->getItem());

        auto delta = static_cast <float> (Global::getCore().getAppTime().getDelta());
        float movingOffset{m_character.getCurrentVelocity().getLength() * delta * k_spreadAngleOffsetDueToMovementMultiplier};
        float newValue{m_spreadAngleDecrease.getCurrentValue() + movingOffset};

        if(newValue > minProjectilesSpreadAngle * k_maxProjectilesSpreadAngleMultiplier)
            newValue = minProjectilesSpreadAngle * k_maxProjectilesSpreadAngleMultiplier;

        m_spreadAngleDecrease.setValueWithoutInterpolation(newValue);
        m_spreadAngleDecrease.setTargetValue(minProjectilesSpreadAngle);
    }

    m_spreadAngle.setTargetValue(m_spreadAngleDecrease.getCurrentValue());
}

void ProjectilesSpreadAngleManager::onUsedItem()
{
    if(m_aiming)
        return; // no recoil when aiming

    E_DASSERT(m_handsItemContainer, "Hands item container is nullptr.");

    if(!m_handsItemContainer->hasItem())
        return;

    float minProjectilesSpreadAngle{CharacterStatsOrSkillsRelatedFormulas::getMinProjectilesSpreadAngle(m_character, m_handsItemContainer->getItem())};
    float recoil{CharacterStatsOrSkillsRelatedFormulas::getRecoil(m_character, m_handsItemContainer->getItem())};
    float newValue{m_spreadAngleDecrease.getCurrentValue() + recoil};

    if(newValue > minProjectilesSpreadAngle * k_maxProjectilesSpreadAngleMultiplier)
        newValue = minProjectilesSpreadAngle * k_maxProjectilesSpreadAngleMultiplier;

    float prevTarget{m_spreadAngleDecrease.getTargetValue()};
    m_spreadAngleDecrease.setValueWithoutInterpolation(newValue);
    m_spreadAngleDecrease.setTargetValue(prevTarget);
}

void ProjectilesSpreadAngleManager::setAiming(bool aiming)
{
    m_aiming = aiming;
}

bool ProjectilesSpreadAngleManager::isAiming() const
{
    return m_aiming;
}

float ProjectilesSpreadAngleManager::getCurrentSpreadAngle() const
{
    E_DASSERT(m_handsItemContainer, "Hands item container is nullptr.");

    if(!m_handsItemContainer->hasItem())
        return 0.f;

    return m_spreadAngle.getCurrentValue();
}

const float ProjectilesSpreadAngleManager::k_spreadAngleInterpolationStep{50.f};
const float ProjectilesSpreadAngleManager::k_spreadAngleDecreaseInterpolationStep{9.4f};
const float ProjectilesSpreadAngleManager::k_maxProjectilesSpreadAngleMultiplier{3.2f};
const float ProjectilesSpreadAngleManager::k_spreadAngleOffsetDueToMovementMultiplier{0.0044f};
const float ProjectilesSpreadAngleManager::k_aimingSpreadAngle{0.2f};

} // namespace app
