#ifndef APP_THIS_PLAYER_HPP
#define APP_THIS_PLAYER_HPP

#include "FPPItemModelController.hpp"
#include "ItemCurrentlyDragged.hpp"
#include "StructureCurrentlyDesignated.hpp"
#include "ItemInHands.hpp"
#include "PointedEntity.hpp"
#include "DeconstructionTracker.hpp"
#include "RevivingTracker.hpp"
#include "ItemContainerSearchTracker.hpp"
#include "SwimTracker.hpp"

#include <memory>

namespace engine { namespace GUI { class Event; } }

namespace app
{

class Character;

class ThisPlayer
{
public:
    ThisPlayer();

    void update();
    void onEvent(engine::GUI::Event &event);

    Character &getCharacter() const;
    const std::shared_ptr <Character> &getCharacterPtr() const;

    FPPItemModelController &getFPPItemModelController();
    const FPPItemModelController &getFPPItemModelController() const;

    ItemCurrentlyDragged &getItemCurrentlyDragged();
    const ItemCurrentlyDragged &getItemCurrentlyDragged() const;

    StructureCurrentlyDesignated &getStructureCurrentlyDesignated();
    const StructureCurrentlyDesignated &getStructureCurrentlyDesignated() const;

    PointedEntity &getPointedEntity();
    const PointedEntity &getPointedEntity() const;

    DeconstructionTracker &getDeconstructionTracker();
    const DeconstructionTracker &getDeconstructionTracker() const;

    RevivingTracker &getRevivingTracker();
    const RevivingTracker &getRevivingTracker() const;

    ItemContainerSearchTracker &getItemContainerSearchTracker();
    const ItemContainerSearchTracker &getItemContainerSearchTracker() const;

private:
    void updateCharacterMovement();
    void updatePlayerMovementTimeAccumulator();

    void playStepSound() const;
    void toggleAiming();
    void reload();

    static constexpr irr::EKEY_CODE k_pickUpItemKey{irr::KEY_KEY_E};
    static constexpr irr::EKEY_CODE k_useWorkbenchKey{irr::KEY_KEY_E};
    static const float k_playerMovementTimeAccumulatorGrowthRate;

    std::shared_ptr <Character> m_character;

    FPPItemModelController m_FPPItemModelController;
    ItemCurrentlyDragged m_itemCurrentlyDragged;
    StructureCurrentlyDesignated m_structureCurrentlyDesignated;
    ItemInHands m_itemInHands;
    PointedEntity m_pointedEntity;
    DeconstructionTracker m_deconstructionTracker;
    RevivingTracker m_revivingTracker;
    ItemContainerSearchTracker m_itemContainerSearchTracker;
    SwimTracker m_swimTracker;

    float m_playerMovementTimeAccumulator;
    float m_lastStepPlayerMovementTimeAccumulator;
};

} // namespace app

#endif // APP_THIS_PLAYER_HPP
