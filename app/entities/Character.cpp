#include "Character.hpp"

#include "engine/app3D/managers/PhysicsManager.hpp"
#include "engine/app3D/managers/SceneManager.hpp"
#include "engine/app3D/physics/DynamicCharacterController.hpp"
#include "engine/app3D/physics/RigidBody.hpp"
#include "engine/app3D/physics/Armature.hpp"
#include "engine/app3D/sceneNodes/Model.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/app3D/IrrlichtConversions.hpp"
#include "engine/util/Util.hpp"
#include "engine/GUI/GUIManager.hpp"
#include "engine/GUI/IGUIRenderer.hpp"
#include "../defs/CharacterDef.hpp"
#include "../defs/ItemDef.hpp"
#include "../defs/CachedCollisionShapeDef.hpp"
#include "../defs/AnimationFramesSetDef.hpp"
#include "../defs/DefsCache.hpp"
#include "../defs/FactionDef.hpp"
#include "../defs/WorldPartDef.hpp"
#include "../itemContainers/SingleSlotItemContainer.hpp"
#include "../itemContainers/MultiSlotItemContainer.hpp"
#include "../entities/Item.hpp"
#include "../GUI/MainGUI.hpp"
#include "../world/World.hpp"
#include "../world/WorldPart.hpp"
#include "../thisPlayer/ThisPlayer.hpp"
#include "../Core.hpp"
#include "../Global.hpp"
#include "../SoundPool.hpp"
#include "../EffectsPool.hpp"
#include "components/PlayerComponent.hpp"
#include "components/MovingOnGroundNPCComponent.hpp"
#include "components/FlyingNPCComponent.hpp"
#include "character/CharacterStatsOrSkillsRelatedFormulas.hpp"
#include "character/ProjectilesSpreadAngleManager.hpp"
#include "Structure.hpp"

namespace app
{

Character::Character(int entityID, const std::shared_ptr <CharacterDef> &def, bool isPlayer)
    : Entity{entityID},
      m_nextItemUseSourcePosOffsetIndex{},
      m_lastTakenDamageTime{},
      m_busyBecauseOfAnimation{},
      m_dontShowModelInWorld{},
      m_def{def},
      m_characterStatsAccumulator{std::make_shared <CharacterStatsAccumulator> ()},
      m_inventory{m_characterStatsAccumulator},
      m_previouslySwimming{},
      m_previouslyUnderWater{},
      m_HP{},
      m_usePoofEffectOnRemovedFromWorld{true}
{
    TRACK;

    if(!m_def)
        throw engine::Exception{"Character def is nullptr."};

    m_projectilesSpreadAngleManager = std::make_unique <ProjectilesSpreadAngleManager> (*this, m_inventory.getHandsItemContainerPtr());

    m_HP = CharacterStatsOrSkillsRelatedFormulas::getMaxHP(*this);

    if(isPlayer)
        m_playerComponent = std::make_unique <PlayerComponent> (*this);
    else {
        if(m_def->canFly())
            m_NPCComponent = std::make_unique <FlyingNPCComponent> (*this);
        else
            m_NPCComponent = std::make_unique <MovingOnGroundNPCComponent> (*this);
    }
}

bool Character::wantsEverInWorldUpdate() const
{
    return true;
}

void Character::setInWorldPosition(const engine::FloatVec3 &pos)
{
    base::setInWorldPosition(pos);

    if(m_characterModel)
        m_characterModel->setPosition(pos + getFlyingAnimationOffset());

    if(m_characterController)
        m_characterController->setPosition(pos);

    if(m_corpseRigidBody)
        m_corpseRigidBody->setPosition(pos);
}

void Character::setInWorldRotation(const engine::FloatVec3 &rot)
{
    base::setInWorldRotation(rot);

    if(m_characterModel)
        m_characterModel->setRotation(rot);

    if(m_corpseRigidBody)
        m_corpseRigidBody->setRotation(rot);

    // here we assume that character controller doesn't need rotation (it doesn't matter how it is rotated)
}

bool Character::wantsToBeRemovedFromWorld() const
{
    E_DASSERT(m_def, "Character def is nullptr.");

    return isKilled() && m_disposeBodyTimer.passed() && m_def->shouldEverDisappearAfterBeingKilled();
}

bool Character::canBeRevived(const Character &doer) const
{
    E_DASSERT(m_def, "Character def is nullptr.");

    return isInWorld() &&
           isKilled() &&
           m_def->canBeRevived() &&
           doer.getFactionDef().getRelation(getFactionDef()) == FactionRelationDef::Relation::Good &&
           m_def->getSkillsRequiredToRevive().isMet(doer);
}

bool Character::isKilled() const
{
    return m_HP <= 0;
}

engine::FloatVec3 Character::getAIAimPosition() const
{
    return (getInWorldPosition() + getEyesPosition()) / 2.f;
}

int Character::getAIPotentialTargetPriority() const
{
    return 2;
}

bool Character::isPotentiallyMeleeReachableBy(const Character &character) const
{
    // we assume that if character is in air, then he'll be reachable when he lands
    // (otherwise jumping character on ground would be considered as potentially unreachable by AI)
    if(m_characterController && !m_characterController->isOnGround())
        return true;

    return base::isPotentiallyMeleeReachableBy(character);
}

bool Character::tryPickUpItem(std::shared_ptr <Item> item)
{
    if(!item)
        throw engine::Exception{"Tried to pick up nullptr item."};

    if(!item->isInWorld())
        return false;

    if(!isPlayer())
        return false;

    auto stack = item->getStack();

    return getInventory().getMultiSlotItemContainer().tryAddItem(item) == stack;
}

std::shared_ptr <EffectDef> Character::getOnHitEffectDefPtr() const
{
    E_DASSERT(m_def, "Character def is nullptr.");
    return m_def->getOnHitEffectDefPtr();
}

std::string Character::getName() const
{
    E_DASSERT(m_def, "Character def is nullptr.");
    return m_def->getCapitalizedLabel();
}

void Character::onInWorldUpdate()
{
    TRACK;

    if(m_characterController) {
        m_characterController->setJumpVelocity(CharacterStatsOrSkillsRelatedFormulas::getJumpVelocity(*this));

        // update entitiy position and model position with physical body position

        makeSureCharacterControllerPosIsInWorldBounds();

        const auto &pos = m_characterController->getPosition();

        base::setInWorldPosition(pos);

        // character model is optional
        if(m_characterModel)
            m_characterModel->setPosition(pos + getFlyingAnimationOffset());
    }
    else if(m_corpseRigidBody) {
        // update entitiy position and model position with corpse physical body position

        const auto &pos = m_corpseRigidBody->getPosition();
        const auto &rot = m_corpseRigidBody->getRotation();

        base::setInWorldPosition(pos);
        base::setInWorldRotation(rot);

        // character model is optional
        if(m_characterModel) {
            m_characterModel->setPosition(pos);
            m_characterModel->setRotation(rot);
        }

        m_corpseRigidBody->affectByWater(WorldPart::k_waterHeight);
    }
    else if(m_armature) {
        if(m_characterModel)
            m_armature->updateModelWithArmature(*m_characterModel);
    }

    E_DASSERT(m_projectilesSpreadAngleManager, "Projectiles spread angle manager is nullptr.");
    m_projectilesSpreadAngleManager->update();

    getCharacterComponent().onInWorldUpdate();

    updateSwimmingSounds();
    updateDamageTimers();
    updateRegenerateHP();
    updateKillWhenTouchedWater();
}

void Character::onSpawnedInWorld()
{
    TRACK;

    base::onSpawnedInWorld();

    auto &core = Global::getCore();
    auto &soundPool = core.getSoundPool();
    const auto &pos = getInWorldPosition();

    E_DASSERT(m_def, "Character def is nullptr.");

    if(!m_dontShowModelInWorld) {
        auto &device = core.getDevice();
        const auto &rot = getInWorldRotation();

        m_characterModel = device.getSceneManager().addModel(m_def->getModelDefPtr());
        m_characterModel->setPosition(pos + getFlyingAnimationOffset());
        m_characterModel->setRotation(rot);
    }

    if(isKilled())
        addCorpse();
    else {
        addDynamicCharacterController();

        if(m_def->hasOnSpawnedSoundDef())
            soundPool.play(m_def->getOnSpawnedSoundDefPtr(), pos);
    }

    if(m_characterModel) {
        if(isKilled()) {
            const auto &deathAnimation = m_def->getAnimationFramesSetDef().getDeath();
            m_characterModel->playAnimationLoop({deathAnimation.to, deathAnimation.to});
        }
        else
            playBusyAnimation(m_def->getAnimationFramesSetDef().getOnSpawned());
    }

    m_usePoofEffectOnRemovedFromWorld = true;
}

void Character::onRemovedFromWorld()
{
    base::onRemovedFromWorld();

    if(m_usePoofEffectOnRemovedFromWorld) {
        E_DASSERT(m_def, "Character def is nullptr.");

        const auto &cachedCollisionShapeDef = m_def->getCachedCollisionShapeDef();
        cachedCollisionShapeDef.addPoofEffect(getInWorldPosition(), getInWorldRotation());

        auto &core = Global::getCore();
        auto &defsCache = core.getDefsCache();
        auto &soundPool = core.getSoundPool();

        soundPool.play(defsCache.Sound_Poof, getInWorldPosition());
    }

    m_characterModel.reset();
    m_characterController.reset();
    m_corpseRigidBody.reset();

    setBusyBecauseOfAnimation(false);
}

void Character::onDraw2DInfoWhenPointed()
{
    auto &core = Global::getCore();
    auto &device = core.getDevice();
    const auto &screenCenter = device.getScreenSize() / 2;
    const auto &GUIRenderer = device.getGUIManager().getRenderer();
    const auto &thisPlayerCharacter = core.getThisPlayer().getCharacter();
    auto pos = screenCenter.moved(35, -20);

    E_DASSERT(m_def, "Character def is nullptr.");

    GUIRenderer.drawText(getName(), pos, {0.5f, 0.5f, 0.5f, 0.8f}, engine::GUI::IGUIRenderer::FontSize::Big);
    pos.y += GUIRenderer.getTextSize(getName(), engine::GUI::IGUIRenderer::FontSize::Big).y;

    int maxHP{CharacterStatsOrSkillsRelatedFormulas::getMaxHP(*this)};

    const auto &hpStr = std::to_string(m_HP) + " / " + std::to_string(maxHP);
    GUIRenderer.drawText(hpStr, pos, {0.7f, 0.2f, 0.2f, 0.8f}, engine::GUI::IGUIRenderer::FontSize::Medium);
    pos.y += GUIRenderer.getTextSize(hpStr, engine::GUI::IGUIRenderer::FontSize::Medium).y;

    if(canBeRevived(thisPlayerCharacter))
        GUIRenderer.drawText("Hold E to revive.", pos, {0.5f, 0.5f, 0.5f, 0.8f}, engine::GUI::IGUIRenderer::FontSize::Medium);
}

void Character::onItemUsedOnMe(Entity &doer, const Item &item)
{
    if(isKilled())
        return;

    const auto &onUsed = item.getDef().getOnUsed();

    int damage{CharacterStatsOrSkillsRelatedFormulas::getDamage(doer, *this, item)};

    decreaseHPBy(damage);

    auto *doerCharacter = dynamic_cast <Character*> (&doer);
    auto *doerStructure = dynamic_cast <Structure*> (&doer);

    if(doerCharacter && doerCharacter->isPlayer()) {
        auto &skills = doerCharacter->getPlayerComponent().getSkills();

        if(!onUsed.isMelee())
            skills.addShootingExp(damage);

        if(isKilled())
            skills.addExp(getDef().getExpPerKill());
    }

    if(doerStructure && doerStructure->hasOwner()) {
        if(isKilled()) {
            auto &owner = doerStructure->getOwner();

            if(owner.isPlayer())
                owner.getPlayerComponent().getSkills().addExp(getDef().getExpPerKill() * k_expMultiplierForOwnerWhenKilledByStructure);
        }
    }
}

void Character::onRevived(Character &doer)
{
    if(!isKilled())
        return;

    m_HP = CharacterStatsOrSkillsRelatedFormulas::getMaxHP(*this);

    addDynamicCharacterController();

    if(doer.isPlayer()) {
        E_DASSERT(m_def, "Character def is nullptr.");

        doer.getPlayerComponent().getSkills().addElectronicsExp(m_def->getElectronicsExpForReviving());
    }
}

void Character::onHitGround(float force)
{
    onStep();

    int damage{CharacterStatsOrSkillsRelatedFormulas::getFallDamage(*this, force)};

    if(damage > 0)
        decreaseHPBy(damage);
}

void Character::onStoppedBusyAnimation()
{
    setBusyBecauseOfAnimation(false);

    if(m_characterModel) {
        E_DASSERT(m_def, "Character def is nullptr.");

        engine::IntRange animation;

        if(isKilled()) {
            const auto &deathAnimation = m_def->getAnimationFramesSetDef().getDeath();
            animation = {deathAnimation.to, deathAnimation.to};
        }
        else
            animation = m_def->getAnimationFramesSetDef().getIdle();

        if(!animation.isEmpty())
            m_characterModel->playAnimationLoop(animation);
    }
}

void Character::tryUseItemInHands(const engine::FloatVec3 &direction, bool rotateToFaceDir)
{
    TRACK;

    if(isKilled())
        return;

    if(m_busyBecauseOfAnimation)
        return;

    auto &handsItemContainer = getInventory().getHandsItemContainer();

    if(!handsItemContainer.hasItem())
        return;

    if(rotateToFaceDir) {
        const auto &facePos = getInWorldPosition() + direction;

        E_DASSERT(m_def, "Character def is nullptr.");

        if(m_def->canFly())
            setInWorldRotationToFace(facePos);
        else
            setInWorldRotationToFace({facePos.x, facePos.z});
    }

    const auto &item = handsItemContainer.getItem();
    const auto &onUsed = item.getDef().getOnUsed();
    const auto &itemUseSourcePos = getNextItemUseSourcePos();

    m_busyTimer.set(onUsed.getMinDuration());

    engine::FloatVec3 rayEnd;

    E_DASSERT(m_projectilesSpreadAngleManager, "Projectile spread angle manager is nullptr.");

    float spread{m_projectilesSpreadAngleManager->getCurrentSpreadAngle()};

    const auto &directionWithOffset = engine::Util::getRandomPointOnSpherePart(direction, spread);

    if(onUsed.isMelee())
        rayEnd = itemUseSourcePos + directionWithOffset * ItemDef::k_meleeRange;
    else
        rayEnd = itemUseSourcePos + directionWithOffset * ItemDef::k_nonMeleeRange;

    engine::FloatVec3 rayHitPos;
    int rayHitEntityID{-1};
    bool didHit{rayTest_notMe(itemUseSourcePos, rayEnd, engine::app3D::CollisionFilter::AllReal | engine::app3D::CollisionFilter::Water, rayHitPos, rayHitEntityID)};

    if(onUsed.hasSoundDef())
        Global::getCore().getSoundPool().play(onUsed.getSoundDefPtr(), itemUseSourcePos);

    E_DASSERT(m_projectilesSpreadAngleManager, "Projectiles spread angle manager is nullptr.");
    m_projectilesSpreadAngleManager->onUsedItem();

    if(didHit)
        item.onUsedOnSomething(itemUseSourcePos, rayHitPos, rayHitEntityID, *this);

    E_DASSERT(m_def, "Character def is nullptr.");
    playBusyAnimation(m_def->getAnimationFramesSetDef().getAttack());

    if(!m_dontShowModelInWorld && onUsed.hasEffectDef()) {
        auto &effectsPool = Global::getCore().getEffectsPool();
        effectsPool.add(onUsed.getEffectDefPtr(), itemUseSourcePos + getFlyingAnimationOffset(), getInWorldRotation());
    }

    ++m_nextItemUseSourcePosOffsetIndex;
}

void Character::dontShowModelInWorld()
{
    m_dontShowModelInWorld = true;
    setBusyBecauseOfAnimation(false);

    if(m_characterModel)
        m_characterModel.reset();
}

void Character::setMovement(const engine::FloatVec3 &movement)
{
    if(!m_characterController)
        return;

    bool movingBefore{!m_characterController->getMovement().isFuzzyZero()};

    if(m_busyBecauseOfAnimation || isKilled()) {
        m_characterController->setMovement({});
        return;
    }
    else
        m_characterController->setMovement(movement);

    if(m_characterModel) {
        if(movingBefore) {
            if(movement.isFuzzyZero()) {
                E_DASSERT(m_def, "Character def is nullptr.");

                const auto &idleAnimation = m_def->getAnimationFramesSetDef().getIdle();

                if(!idleAnimation.isEmpty())
                    m_characterModel->playAnimationLoop(idleAnimation);
            }
        }
        else if(!m_characterController->getMovement().isFuzzyZero()) {
            E_DASSERT(m_def, "Character def is nullptr.");

            const auto &runAnimation = m_def->getAnimationFramesSetDef().getRun();

            if(!runAnimation.isEmpty())
                m_characterModel->playAnimationLoop(runAnimation);
        }
    }
}

void Character::setMovement2D(const engine::FloatVec2 &movement)
{
    setMovement({movement.x, 0.f, movement.y});
}

bool Character::canJump() const
{
    if(!m_characterController)
        return false;

    return m_characterController->canJump();
}

void Character::jump()
{
    if(m_characterController && canJump()) {
        m_characterController->jump();
        onStep();
    }
}

void Character::onStep()
{
    if(!m_characterController || !m_characterController->isOnGround())
        return;

    if(m_stepTimer.passed())
        m_stepTimer.set(k_minTimeBetweenSteps);
    else
        return;

    auto &core = Global::getCore();
    auto &soundPool = core.getSoundPool();
    const auto &defsCache = core.getDefsCache();
    const auto &pos = getInWorldPosition();
    const auto &world = core.getWorld();

    int entityID{getEntityIDOnWhichWalkedPreviousFrame()};

    if(entityID < 0) {
        auto type = world.getGroundType({pos.x, pos.z});

        if(type == GroundType::Ground1 || type == GroundType::Ground2 || type == GroundType::Ground3 ||
           type == GroundType::Slope) {
            auto *worldPart = world.getWorldPart({pos.x, pos.z});

            if(worldPart) {
                if(type == GroundType::Ground1)
                    soundPool.play(worldPart->getDef().getGround1StepSoundDefPtr(), pos);
                else if(type == GroundType::Ground2)
                    soundPool.play(worldPart->getDef().getGround2StepSoundDefPtr(), pos);
                else if(type == GroundType::Ground3)
                    soundPool.play(worldPart->getDef().getGround3StepSoundDefPtr(), pos);
                else // slope
                    soundPool.play(worldPart->getDef().getSlopeStepSoundDefPtr(), pos);
            }
        }
        else if(type == GroundType::UnderWater)
            soundPool.play(defsCache.Sound_WaterStep, pos);
    }
    else if(world.entityExists(entityID))
        world.getEntity(entityID).onCharacterStepOnIt(*this);
}

bool Character::rayTest_notMe(const engine::FloatVec3 &start, const engine::FloatVec3 &end, engine::app3D::CollisionFilter withWhatCollide, engine::FloatVec3 &outPos, int &outHitBodyUserIndex) const
{
    TRACK;

    auto &physicsManager = Global::getCore().getDevice().getPhysicsManager();

    if(m_characterController)
        return m_characterController->rayTest_notMe(physicsManager, start, end, withWhatCollide, outPos, outHitBodyUserIndex);
    else if(m_corpseRigidBody)
        return physicsManager.rayTest_notMe(start, end, m_corpseRigidBody, withWhatCollide, outPos, outHitBodyUserIndex);
    else
        return physicsManager.rayTest(start, end, withWhatCollide, outPos, outHitBodyUserIndex);
}

CharacterDef &Character::getDef() const
{
    E_DASSERT(m_def, "Character def is nullptr.");

    return *m_def;
}

float Character::getMsSinceLastTakenDamage() const
{
    const auto &appTime = Global::getCore().getAppTime();

    return appTime.getElapsedMs() - m_lastTakenDamageTime;
}

float Character::getDistanceBetweenFeetAndEyes() const
{
    E_DASSERT(m_def, "Character def is nullptr.");

    return m_def->getCachedCollisionShapeDef().getHeight();
}

bool Character::isBusy() const
{
    return m_busyBecauseOfAnimation || !m_busyTimer.passed();
}

bool Character::isSwimming() const
{
    if(m_characterController)
        return m_characterController->isSwimming();

    return false;
}

bool Character::isUnderWater() const
{
    if(m_characterController)
        return m_characterController->isUnderWater();

    return false;
}

bool Character::wasWalkingPreviousFrame() const
{
    if(m_characterController)
        return m_characterController->wasWalkingPreviousFrame();

    return false;
}

int Character::getEntityIDOnWhichWalkedPreviousFrame() const
{
    if(!wasWalkingPreviousFrame())
        return -1;

    if(m_characterController)
        return m_characterController->getBodyUserIndexOnWhichWalkedPreviousFrame();

    return -1;
}

bool Character::isPlayer() const
{
    return static_cast <bool> (m_playerComponent);
}

bool Character::isNPC() const
{
    return static_cast <bool> (m_NPCComponent);
}

int Character::getHP() const
{
    return m_HP;
}

ProjectilesSpreadAngleManager &Character::getProjectilesSpreadAngleManager() const
{
    E_DASSERT(m_projectilesSpreadAngleManager, "Projectiles spread angle manager is nullptr.");

    return *m_projectilesSpreadAngleManager;
}

engine::FloatVec3 Character::getEyesPosition() const
{
    // note: the previous version was + rotateAsMe({0.f, getDistanceBetweenFeetAndEyes(), 0.f});
    // but for some reason it was wrong (flying sphere bot couldn't hit wall)
    // maybe it was not wrong, but just flying bots eyes were inside the wall? I don't know
    // (by that time eyes position == attack source position)
    if(m_characterController)
        return m_characterController->getPosition().movedY(getDistanceBetweenFeetAndEyes());

    return {};
}

engine::FloatVec3 Character::getHorizontalLookVec() const
{
    const auto &irrVec = engine::app3D::IrrlichtConversions::toVector(getInWorldRotation()).rotationToDirection();

    return engine::FloatVec3{irrVec.X, 0.f, irrVec.Z}.normalized();
}

engine::FloatVec3 Character::getCurrentVelocity() const
{
    if(!m_characterController)
        return {};

    return m_characterController->getLinearVelocity();
}

engine::FloatVec3 Character::getNextItemUseSourcePos()
{
    E_DASSERT(m_def, "Character def is nullptr.");

    const auto &offsets = m_def->getItemUseSourcePosOffsets();

    if(offsets.empty())
        return getEyesPosition();

    if(m_nextItemUseSourcePosOffsetIndex < 0 ||
       m_nextItemUseSourcePosOffsetIndex >= static_cast <int> (offsets.size()))
        m_nextItemUseSourcePosOffsetIndex = 0;

    return getInWorldPosition() + rotateAsMe(offsets[m_nextItemUseSourcePosOffsetIndex]);
}

Inventory &Character::getInventory()
{
    return m_inventory;
}

const Inventory &Character::getInventory() const
{
    return m_inventory;
}

CharacterStatsAccumulator &Character::getCharacterStatsAccumulator() const
{
    E_DASSERT(m_characterStatsAccumulator, "Character stats accumulator is nullptr.");
    return *m_characterStatsAccumulator;
}

const std::shared_ptr <CharacterStatsAccumulator> &Character::getCharacterStatsAccumulatorPtr() const
{
    E_DASSERT(m_characterStatsAccumulator, "Character stats accumulator is nullptr.");
    return m_characterStatsAccumulator;
}

PlayerComponent &Character::getPlayerComponent() const
{
    if(!m_playerComponent)
        throw engine::Exception{"Tried to get nullptr player component. This should have been checked before."};

    return *m_playerComponent;
}

NPCComponent &Character::getNPCComponent() const
{
    if(!m_NPCComponent)
        throw engine::Exception{"Tried to get nullptr NPC component. This should have been checked before."};

    return *m_NPCComponent;
}

Character::~Character()
{
}

void Character::updateSwimmingSounds()
{
    if(isKilled())
        return;

    auto &core = Global::getCore();
    auto &defsCache = core.getDefsCache();
    auto &soundPool = core.getSoundPool();

    bool swimming{isSwimming()};
    bool underWater{isUnderWater()};

    if(m_characterController) {
        if(swimming && !m_previouslySwimming) {
            if(m_startSwimmingSoundCooldownTimer.passed()) {
                if(m_characterController->getLinearVelocity().y < -10.f)
                    soundPool.play(defsCache.Sound_FallIntoWater, getInWorldPosition());
                else
                    soundPool.play(defsCache.Sound_StartSwimming, getInWorldPosition());

                m_startSwimmingSoundCooldownTimer.set(k_startSwimmingSoundCooldownTime);
            }

            m_swimmingSoundTimer.set(k_timeBetweenSwimmingForwardSounds);
        }

        if(!swimming && m_previouslySwimming && m_leaveWaterSoundCooldownTimer.passed()) {
            soundPool.play(defsCache.Sound_LeaveWater, getInWorldPosition());
            m_leaveWaterSoundCooldownTimer.set(k_leaveWaterSoundCooldownTime);
        }

        if(!underWater && m_previouslyUnderWater)
            m_swimmingSoundTimer.set(k_timeBetweenSwimmingForwardSounds);

        if(swimming && !isUnderWater() && m_swimmingSoundTimer.passed()) {
            if(m_characterController->wasTryingToMovePreviousFrame()) {
                soundPool.play(defsCache.Sound_Swim1, getInWorldPosition());
                m_swimmingSoundTimer.set(k_timeBetweenSwimmingForwardSounds);
            }
            else {
                soundPool.play(defsCache.Sound_Swim2, getInWorldPosition());
                m_swimmingSoundTimer.set(k_timeBetweenSwimmingStillSounds);
            }
        }
    }

    m_previouslySwimming = swimming;
    m_previouslyUnderWater = underWater;
}

void Character::updateDamageTimers()
{
    if(isKilled())
        return;

    E_DASSERT(m_def, "Character def is nullptr.");

    if(m_damageInWaterTimer.passed()) {
        m_damageInWaterTimer.set(5000.0);

        if(isSwimming())
            decreaseHPBy(m_def->getDamageInWaterPer5Seconds());
    }

    if(m_damageDuringDayTimer.passed()) {
        m_damageDuringDayTimer.set(5000.0);

        float hours{Global::getCore().getWorld().getDateTimeManager().getTime().getHoursAsFloat()};

        if(k_damageDuringDayHoursRange.isInRange(hours))
            decreaseHPBy(m_def->getDamageDuringDayPer5Seconds());
    }
}

void Character::updateRegenerateHP()
{
    if(isKilled())
        return;

    if(m_regenerateHPPer5SecTimer.passed()) {
        m_regenerateHPPer5SecTimer.set(5000.0);
        increaseHPBy(CharacterStatsOrSkillsRelatedFormulas::getHPRegenerationPer5Sec(*this));
    }
}

void Character::updateKillWhenTouchedWater()
{
    E_DASSERT(m_def, "Character def is nullptr.");

    if(!m_def->killWhenTouchedWater())
        return;

    if(isKilled())
        return;

    if(getInWorldPosition().y <= WorldPart::k_waterHeight)
        decreaseHPBy(m_HP);
}

CharacterComponent &Character::getCharacterComponent() const
{
    if(m_NPCComponent)
        return *m_NPCComponent;

    if(m_playerComponent)
        return *m_playerComponent;

    throw engine::Exception{"Character is neither NPC nor player."};
}

void Character::setBusyBecauseOfAnimation(bool busy)
{
    if(busy && m_characterController)
        m_characterController->setMovement({});

    m_busyBecauseOfAnimation = busy;
}

void Character::decreaseHPBy(int by)
{
    if(by <= 0)
        return;

    if(isKilled())
        return;

    m_HP -= by;

    if(m_HP < 0)
        m_HP = 0;

    auto &core = Global::getCore();
    const auto &defsCache = core.getDefsCache();
    auto &soundPool = core.getSoundPool();

    m_lastTakenDamageTime = core.getAppTime().getElapsedMs();

    if(m_HP == 0)
        onKilled();

    // play animation
    if(m_characterModel) {
        E_DASSERT(m_def, "Character def is nullptr.");

        engine::IntRange animation;

        if(m_HP == 0)
            animation = m_def->getAnimationFramesSetDef().getDeath();
        else
            animation = m_def->getAnimationFramesSetDef().getHarmed();

        playBusyAnimation(animation);
    }

    if(m_HP > 0 && isItThisPlayersCharacter()) {
        soundPool.play(defsCache.Sound_ITakeDamage);
        core.getMainGUI().tryAddBloodSplat();
    }
}

void Character::increaseHPBy(int by)
{
    if(by <= 0)
        return;

    if(m_HP <= 0)
        return; // can't increase HP if dead

    m_HP += by;

    int maxHP{CharacterStatsOrSkillsRelatedFormulas::getMaxHP(*this)};

    if(m_HP > maxHP)
        m_HP = maxHP;
}

void Character::playBusyAnimation(const engine::IntRange &animation)
{
    if(animation.isEmpty())
        return;

    if(!m_characterModel)
        return;

    setBusyBecauseOfAnimation(true);

    m_characterModel->playSingleAnimation(animation, [entityID = getEntityID()] {
        auto &world = Global::getCore().getWorld();

        if(world.entityExists(entityID))
            world.getEntity(entityID).onStoppedBusyAnimation();
    });
}

bool Character::isItThisPlayersCharacter() const
{
    return this == &Global::getCore().getThisPlayer().getCharacter();
}

engine::FloatVec3 Character::getFlyingAnimationOffset() const
{
    E_DASSERT(m_def, "Character def is nullptr.");

    if(!m_def->canFly())
        return {};

    double elapsedMs{Global::getCore().getAppTime().getElapsedMs()};

    return {0.f, static_cast <float> (std::sin(getEntityID() / 10.235623f + elapsedMs * k_flyingAnimationOffsetElapsedMsSinMultiplier) * k_flyingAnimationOffsetAmplitude / 2.f), 0.f};
}

void Character::onKilled()
{
    setMovement({});
    m_characterController.reset();

    auto &core = Global::getCore();
    auto &soundPool = core.getSoundPool();
    auto &defsCache = core.getDefsCache();
    auto &physicsManager = core.getDevice().getPhysicsManager();

    /*
     * TODO!
    if(m_characterModel) {
        m_characterModel->enableManualJointManipulation();
        m_armature = physicsManager.addArmature(*m_characterModel);
    }
    */

     addCorpse();

    if(isItThisPlayersCharacter())
        soundPool.play(defsCache.Sound_MyDeath);

    if(m_def->shouldEverDisappearAfterBeingKilled()) {
        // handle effects for characters that will eventually disappear after some time

        if(m_def->hasAnyOnKilledEffect()) {
            const auto &onKilledEffect = m_def->getRandomOnKilledEffect();
            const auto &offset = m_def->getCachedCollisionShapeDef().getPosOffset();

            auto &effectsPool = core.getEffectsPool();
            effectsPool.add(onKilledEffect.getEffectDefPtr(), getInWorldPosition() + getFlyingAnimationOffset() + offset, getInWorldRotation());

            m_disposeBodyTimer.set(onKilledEffect.getDisposeBodyTime());
            m_usePoofEffectOnRemovedFromWorld = (onKilledEffect.getDisposeBodyTime() > 0.f);
        }
        else {
            m_disposeBodyTimer.set(k_defaultTimeBeforeDisposingBody);
            m_usePoofEffectOnRemovedFromWorld = true;
        }
    }
    else {
        // handle effects for characters that will never disappear

        // we don't want on killed effects which normally make character disappear instantly (like explosion),
        // because they usually don't look good for characters whose bodies never disappear
        if(m_def->hasAnyNonInstantOnKilledEffect()) {
            const auto &onKilledEffect = m_def->getRandomNonInstantOnKilledEffect();
            const auto &offset = m_def->getCachedCollisionShapeDef().getPosOffset();

            auto &effectsPool = core.getEffectsPool();
            effectsPool.add(onKilledEffect.getEffectDefPtr(), getInWorldPosition() + getFlyingAnimationOffset() + offset, getInWorldRotation());
        }
    }
}

void Character::addCorpse()
{
    E_DASSERT(m_def, "Character def is nullptr.");

    if(m_def->getOnKilledAction() == CharacterDef::OnKilledAction::MakeBodyDynamic) {
        auto &core = Global::getCore();
        const auto &cachedCollisionShapeDef = m_def->getCachedCollisionShapeDef();
        const auto &shape = cachedCollisionShapeDef.getCollisionShapePtr();
        const auto &posOffset = cachedCollisionShapeDef.getPosOffset();

        m_corpseRigidBody = core.getDevice().getPhysicsManager().addRigidBody(shape, m_def->getMass(), getEntityID(), posOffset);
        m_corpseRigidBody->setPosition(getInWorldPosition() + getFlyingAnimationOffset());
        m_corpseRigidBody->setRotation(getInWorldRotation());
        m_corpseRigidBody->setFriction(0.5f, 0.1f);
    }
}

void Character::addDynamicCharacterController()
{
    E_DASSERT(m_def, "Character def is nullptr.");

    auto &device = Global::getCore().getDevice();
    const auto &cachedCollisionShapeDef = m_def->getCachedCollisionShapeDef();
    const auto &shape = cachedCollisionShapeDef.getCollisionShapePtr();
    const auto &posOffset = cachedCollisionShapeDef.getPosOffset();

    m_characterController = device.getPhysicsManager().addDynamicCharacterController(shape, m_def->getMass(), getDistanceBetweenFeetAndEyes(), WorldPart::k_waterHeight, m_def->canFly(), getEntityID(), posOffset);
    m_characterController->setPosition(getInWorldPosition());
    // here we assume that character controller doesn't need rotation (it doesn't matter how it is rotated)
    m_characterController->setJumpVelocity(CharacterStatsOrSkillsRelatedFormulas::getJumpVelocity(*this));

    m_characterController->setOnHitGroundCallback([entityID = getEntityID()](float force) {
        auto &world = Global::getCore().getWorld();

        if(world.entityExists(entityID))
            world.getEntity(entityID).onHitGround(force);
    });
}

void Character::makeSureCharacterControllerPosIsInWorldBounds()
{
    if(!m_characterController)
        return;

    auto pos = m_characterController->getPosition();
    const auto &worldBounds = Global::getCore().getWorld().getBounds();
    bool changed{};

    if(pos.x > worldBounds.getMaxX()) {
        pos.x = worldBounds.getMaxX();
        changed = true;
    }

    if(pos.x < worldBounds.pos.x) {
        pos.x = worldBounds.pos.x;
        changed = true;
    }

    if(pos.z > worldBounds.getMaxY()) { // 2d vs 3d
        pos.z = worldBounds.getMaxY(); // 2d vs 3d
        changed = true;
    }

    if(pos.z < worldBounds.pos.y) { // 2d vs 3d
        pos.z = worldBounds.pos.y; // 2d vs 3d
        changed = true;
    }

    if(changed)
        m_characterController->setPosition(pos);
}

const float Character::k_minTimeBetweenSteps{200.f};
const float Character::k_timeBetweenSwimmingForwardSounds{750.f};
const float Character::k_timeBetweenSwimmingStillSounds{1400.f};
const float Character::k_defaultTimeBeforeDisposingBody{10000.f};
const float Character::k_startSwimmingSoundCooldownTime{500.f};
const float Character::k_leaveWaterSoundCooldownTime{550.f};
const float Character::k_expMultiplierForOwnerWhenKilledByStructure{0.2f};
const float Character::k_flyingAnimationOffsetElapsedMsSinMultiplier{0.002f};
const float Character::k_flyingAnimationOffsetAmplitude{0.3f};
const engine::FloatRange Character::k_damageDuringDayHoursRange{8.f, 15.f};

} // namespace app
