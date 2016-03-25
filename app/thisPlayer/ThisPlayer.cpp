#include "ThisPlayer.hpp"

#include "../entities/character/ProjectilesSpreadAngleManager.hpp"
#include "../entities/character/CharacterStatsOrSkillsRelatedFormulas.hpp"
#include "../entities/components/PlayerComponent.hpp"
#include "../entities/components/NPCComponent.hpp"
#include "engine/app3D/defs/SoundDef.hpp"
#include "engine/app3D/managers/EventManager.hpp"
#include "engine/app3D/managers/SceneManager.hpp"
#include "../entities/Character.hpp"
#include "../entities/Item.hpp"
#include "../entities/Structure.hpp"
#include "../world/World.hpp"
#include "../world/WorldPart.hpp"
#include "../defs/DefsCache.hpp"
#include "../defs/CharacterDef.hpp"
#include "../defs/ItemDef.hpp"
#include "../defs/StructureDef.hpp"
#include "../GUI/MainGUI.hpp"
#include "../itemContainers/SingleSlotItemContainer.hpp"
#include "engine/app3D/IrrlichtConversions.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/util/DefDatabase.hpp"
#include "engine/GUI/Event.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "../EventReceiver.hpp"
#include "../SoundPool.hpp"

namespace app
{

ThisPlayer::ThisPlayer()
    : m_playerMovementTimeAccumulator{},
      m_lastStepPlayerMovementTimeAccumulator{}
{
    TRACK;

    auto &core = Global::getCore();
    auto &world = core.getWorld();
    const auto &defsCache = core.getDefsCache();

    const auto &def = defsCache.Character_Human;
    m_character = std::make_shared <Character> (world.getUniqueEntityID(), def, true);

    const auto &startingPos = world.getPlayerStartingPosition();

    float height{world.getHeight(startingPos)};
    engine::FloatVec3 startingPos3D{startingPos.x, height + 1.f, startingPos.y};

    m_character->setFactionDef(defsCache.Faction_Players);
    m_character->setInWorldPosition(startingPos3D);
    m_character->dontShowModelInWorld();

    world.addEntity(m_character);
}

void ThisPlayer::update()
{
    TRACK;

    updateCharacterMovement();
    updatePlayerMovementTimeAccumulator();
    m_swimTracker.update();
    m_pointedEntity.update();
    m_structureCurrentlyDesignated.update();
    m_itemInHands.update();
    m_deconstructionTracker.update();
    m_revivingTracker.update();
    m_itemContainerSearchTracker.update();

    auto &character = getCharacter();
    auto &handsItemContainer = character.getInventory().getHandsItemContainer();

    character.setInWorldRotation({0.f, Global::getCore().getDevice().getSceneManager().getCameraRotation().y, 0.f});

    if(handsItemContainer.hasItem() && !m_structureCurrentlyDesignated.hasStructure() && !character.isUnderWater())
        m_FPPItemModelController.update(handsItemContainer.getItemPtr(), m_playerMovementTimeAccumulator);
    else
        m_FPPItemModelController.update(nullptr, m_playerMovementTimeAccumulator);
}

void ThisPlayer::onEvent(engine::GUI::Event &event)
{
    TRACK;

    auto &character = getCharacter();

    switch(event.getType()) {
    case engine::GUI::Event::Type::KeyboardEvent:
        switch(event.getKeyCode()) {
        case irr::KEY_SPACE:
            character.jump();
            break;

        case irr::KEY_KEY_R:
            reload();
            break;

        default:
            // here, assigned keys can repeat

            if(event.getKeyCode() == DeconstructionTracker::k_deconstructKey) {
                if(!m_deconstructionTracker.isDeconstructingAnything() &&
                   m_pointedEntity.isAny() &&
                   m_pointedEntity.getEntity().canBeDeconstructed(character))
                        m_deconstructionTracker.startDeconstructing(m_pointedEntity.getEntityPtr());
            }

            if(event.getKeyCode() == RevivingTracker::k_reviveKey) {
                if(!m_revivingTracker.isRevivingAnything() &&
                   m_pointedEntity.isAny() &&
                   m_pointedEntity.getEntity().canBeRevived(character))
                    m_revivingTracker.startReviving(m_pointedEntity.getEntityPtr());
            }

            if(event.getKeyCode() == ItemContainerSearchTracker::k_searchKey) {
                if(!m_itemContainerSearchTracker.isSearchingAnything() &&
                   m_pointedEntity.isAny() &&
                   m_pointedEntity.getEntity().hasSearchableItemContainer()) {
                    m_itemContainerSearchTracker.startSearching(m_pointedEntity.getEntityPtr());

                    auto &core = Global::getCore();
                    auto &defsCache = core.getDefsCache();
                    auto &soundPool = core.getSoundPool();
                    auto &mainGUI = core.getMainGUI();

                    soundPool.play(defsCache.Sound_SearchItemContainer);

                    mainGUI.setGUIMode(true);
                    mainGUI.openInventoryWindow();
                }
            }

            if(event.getKeyCode() == k_pickUpItemKey) {
                if(m_pointedEntity.isAny() &&
                   dynamic_cast <Item*> (&m_pointedEntity.getEntity())) { // oh my... this is just... eh...

                    // TODO: maybe Entity::onPickedUp(Character), (also called when picked up because of pick-up-sphere)

                    auto &world = Global::getCore().getWorld();
                    auto entityID = m_pointedEntity.getEntity().getEntityID();

                    getCharacter().tryPickUpItem(world.getEntityPtrAndCast <Item> (entityID));
                    world.removeEntity(entityID);
                }
            }

            if(event.getKeyCode() == k_useWorkbenchKey) {
                if(m_pointedEntity.isAny() &&
                   dynamic_cast <Structure*> (&m_pointedEntity.getEntity())) { // it's the last time... I promise...

                    auto *structure = static_cast <Structure*> (&m_pointedEntity.getEntity());

                    if(structure->getDef().isWorkbench()) {
                        auto &core = Global::getCore();
                        auto &world = core.getWorld();
                        auto &mainGUI = core.getMainGUI();
                        auto entityID = structure->getEntityID();

                        mainGUI.setGUIMode(true);
                        mainGUI.openCraftingWindow(world.getEntityPtrAndCast <Structure> (entityID));
                    }
                }
            }
        }

        break;

    case engine::GUI::Event::Type::MouseEvent:
        if(event.getMouseAction() == engine::GUI::Event::MouseAction::Down) {
            if(event.getMouseButton() == engine::GUI::Event::MouseButton::Left) {
                m_itemInHands.tryUse();
                m_itemInHands.tryStartContinuousUse();
            }
            else if(event.getMouseButton() == engine::GUI::Event::MouseButton::Right)
                toggleAiming();
        }
        else if(event.getMouseAction() == engine::GUI::Event::MouseAction::Up) {
            if(event.getMouseButton() == engine::GUI::Event::MouseButton::Left) {
                // try build structure if applicable

                auto &structureCurrentlyDesignated = getStructureCurrentlyDesignated();

                if(structureCurrentlyDesignated.hasStructure()) {
                    auto &core = Global::getCore();
                    auto &defsCache = core.getDefsCache();
                    auto &soundPool = core.getSoundPool();

                    if(structureCurrentlyDesignated.tryBuild())
                        soundPool.play(defsCache.Sound_Constructed, character.getInWorldPosition());
                    else
                        soundPool.play(defsCache.Sound_Negative1);
                }

                // stop continuous use if applicable

                m_itemInHands.tryStopContinuousUse();

                // the place where we are currently is lower in hierarchy than
                // engine level GUI event receiver, so it means that Up event was already
                // propagated, and no GUI widget wanted to get currently dragged item, so
                // at this point we just remove it (if there is one)

                m_itemCurrentlyDragged.removeItem();
            }
        }
        break;

    default:
        break;
    }
}

Character &ThisPlayer::getCharacter() const
{
    E_DASSERT(m_character, "Player character is nullptr.");

    return *m_character;
}

const std::shared_ptr <Character> &ThisPlayer::getCharacterPtr() const
{
    E_DASSERT(m_character, "Player character is nullptr.");

    return m_character;
}

FPPItemModelController &ThisPlayer::getFPPItemModelController()
{
    return m_FPPItemModelController;
}

const FPPItemModelController &ThisPlayer::getFPPItemModelController() const
{
    return m_FPPItemModelController;
}

ItemCurrentlyDragged &ThisPlayer::getItemCurrentlyDragged()
{
    return m_itemCurrentlyDragged;
}

const ItemCurrentlyDragged &ThisPlayer::getItemCurrentlyDragged() const
{
    return m_itemCurrentlyDragged;
}

StructureCurrentlyDesignated &ThisPlayer::getStructureCurrentlyDesignated()
{
    return m_structureCurrentlyDesignated;
}

const StructureCurrentlyDesignated &ThisPlayer::getStructureCurrentlyDesignated() const
{
    return m_structureCurrentlyDesignated;
}

PointedEntity &ThisPlayer::getPointedEntity()
{
    return m_pointedEntity;
}

const PointedEntity &ThisPlayer::getPointedEntity() const
{
    return m_pointedEntity;
}

DeconstructionTracker &ThisPlayer::getDeconstructionTracker()
{
    return m_deconstructionTracker;
}

const DeconstructionTracker &ThisPlayer::getDeconstructionTracker() const
{
    return m_deconstructionTracker;
}

RevivingTracker &ThisPlayer::getRevivingTracker()
{
    return m_revivingTracker;
}

const RevivingTracker &ThisPlayer::getRevivingTracker() const
{
    return m_revivingTracker;
}

ItemContainerSearchTracker &ThisPlayer::getItemContainerSearchTracker()
{
    return m_itemContainerSearchTracker;
}

const ItemContainerSearchTracker &ThisPlayer::getItemContainerSearchTracker() const
{
    return m_itemContainerSearchTracker;
}

void ThisPlayer::updateCharacterMovement()
{
    TRACK;

    auto &core = Global::getCore();
    auto &device = core.getDevice();
    auto &eventManager = device.getEventManager();
    const auto &mainGUI = core.getMainGUI();

    engine::FloatVec3 movement;

    if(!mainGUI.isInGUIMode()) {
        // we use real-time input information here, however it's a
        // little bit hackish and could be integrated with EventReceiver

        if(eventManager.isKeyPressed(irr::KEY_KEY_S))
            movement.z = -1.f;

        if(eventManager.isKeyPressed(irr::KEY_KEY_W))
            movement.z = 1.f;

        if(eventManager.isKeyPressed(irr::KEY_KEY_A))
            movement.x = -1.f;

        if(eventManager.isKeyPressed(irr::KEY_KEY_D))
            movement.x = 1.f;
    }

    E_DASSERT(m_character, "Character is nullptr.");

    if(!movement.isFuzzyZero()) {
        const auto &cameraRotation = device.getSceneManager().getCameraRotation();

        movement.normalize();

        bool limitToXZ{};

        if(!m_character->isSwimming())
            limitToXZ = true;
        else if(!m_character->isUnderWater()) {
            irr::core::matrix4 mat;
            mat.setRotationDegrees(engine::app3D::IrrlichtConversions::toVector(cameraRotation));
            auto vec = engine::app3D::IrrlichtConversions::toVector(movement);
            mat.rotateVect(vec);

            if(vec.dotProduct({0.f, -1.f, 0.f}) < 0.6f)
                limitToXZ = true;
        }

        movement *= CharacterStatsOrSkillsRelatedFormulas::getMoveSpeed(*m_character);

        if(limitToXZ) {
            float camRot{engine::Math::degToRad(cameraRotation.y)};

            double moveX{movement.x * cos(-camRot) + movement.z * cos(-camRot + engine::Math::k_pi / 2.f)};
            double moveZ{movement.x * sin(-camRot) + movement.z * sin(-camRot + engine::Math::k_pi / 2.f)};

            movement.x = moveX;
            movement.z = moveZ;
        }
        else {
            irr::core::matrix4 mat;
            mat.setRotationDegrees(engine::app3D::IrrlichtConversions::toVector(cameraRotation));

            auto vec = engine::app3D::IrrlichtConversions::toVector(movement);

            mat.rotateVect(vec);

            movement = {vec.X, vec.Y, vec.Z};
        }

        m_character->setMovement(movement);
    }
    else
        m_character->setMovement({});
}

void ThisPlayer::updatePlayerMovementTimeAccumulator()
{
    // update accumulator

    auto &core = Global::getCore();

    if(getCharacter().wasWalkingPreviousFrame()) {
        double delta{core.getAppTime().getDelta()};
        m_playerMovementTimeAccumulator += delta * k_playerMovementTimeAccumulatorGrowthRate;
    }
    else {
        m_playerMovementTimeAccumulator = 0.f;
        m_lastStepPlayerMovementTimeAccumulator = -1.f;
    }

    // play sound

    float t{std::fmod(m_playerMovementTimeAccumulator, 0.5f)};

    if(t > 0.3f &&
       (std::fabs(m_playerMovementTimeAccumulator - m_lastStepPlayerMovementTimeAccumulator) >= 0.5f ||
        engine::Math::fuzzyCompare(m_playerMovementTimeAccumulator, m_lastStepPlayerMovementTimeAccumulator))) {
        m_lastStepPlayerMovementTimeAccumulator = m_playerMovementTimeAccumulator - (t - 0.3f);
        playStepSound();
    }
}

void ThisPlayer::playStepSound() const
{
    auto &character = getCharacter();

    if(!character.wasWalkingPreviousFrame())
        return;

    character.onStep();
}

void ThisPlayer::toggleAiming()
{
    if(m_FPPItemModelController.toggleAiming()) {
        auto &core = Global::getCore();
        auto &soundPool = core.getSoundPool();
        auto &defsCache = core.getDefsCache();

        if(m_FPPItemModelController.isAiming())
            soundPool.play(defsCache.Sound_StartAiming);
        else
            soundPool.play(defsCache.Sound_StopAiming);
    }
}

void ThisPlayer::reload()
{
    if(m_itemInHands.hasItem()) {
        bool reloaded{m_FPPItemModelController.reload()};

        const auto &itemDef = m_itemInHands.getItem().getDef();

        if(reloaded && itemDef.hasReloadSoundDef())
            Global::getCore().getSoundPool().play(itemDef.getReloadSoundDefPtr(), m_character);
    }
}

const float ThisPlayer::k_playerMovementTimeAccumulatorGrowthRate{0.001f};

} // namespace app
