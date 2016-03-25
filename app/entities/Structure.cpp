#include "Structure.hpp"

#include "engine/app3D/sceneNodes/Model.hpp"
#include "engine/app3D/physics/RigidBody.hpp"
#include "engine/app3D/managers/SceneManager.hpp"
#include "engine/app3D/managers/PhysicsManager.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/GUI/GUIManager.hpp"
#include "engine/GUI/IGUIRenderer.hpp"
#include "../defs/DefsCache.hpp"
#include "../defs/StructureDef.hpp"
#include "../defs/ItemDef.hpp"
#include "../defs/CachedCollisionShapeDef.hpp"
#include "../itemContainers/MultiSlotItemContainer.hpp"
#include "../world/World.hpp"
#include "../world/WorldPart.hpp"
#include "../world/ElectricitySystem.hpp"
#include "../world/Effect.hpp"
#include "../thisPlayer/ThisPlayer.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "../Snapper.hpp"
#include "../SoundPool.hpp"
#include "../EffectsPool.hpp"
#include "components/TurretComponent.hpp"
#include "components/ElectricityComponent.hpp"
#include "character/CharacterStatsOrSkillsRelatedFormulas.hpp"
#include "Character.hpp"
#include "Item.hpp"

namespace app
{

Structure::Structure(int entityID, const std::shared_ptr <StructureDef> &def)
    : Entity{entityID},
      m_def{def},
      m_ownerEntityID{-1},
      m_HP{},
      m_shouldExplodeWhenRemovedFromWorld{}
{
    TRACK;

    if(!m_def)
        throw engine::Exception{"Structure def is nullptr."};

    m_HP = m_def->getMaxHP();

    if(m_def->getTurretInfo().isTurret())
        m_turretComponent = std::make_unique <TurretComponent> (*this);

    if(m_def->usesElectricity())
        m_electricityComponent = std::make_unique <ElectricityComponent> (*this);

    if(m_def->hasSearchableItemContainer())
        m_searchableItemContainer = std::make_shared <MultiSlotItemContainer> (k_searchableItemContainerSize);
}

bool Structure::wantsEverInWorldUpdate() const
{
    E_DASSERT(m_def, "Structure def is nullptr.");

    return m_def->hasMass() || m_def->getTurretInfo().isTurret();
}

void Structure::setInWorldPosition(const engine::FloatVec3 &pos)
{
    base::setInWorldPosition(pos);

    if(m_model)
        m_model->setPosition(pos);

    if(m_rigidBody)
        m_rigidBody->setPosition(pos);

    if(m_effect) {
        E_DASSERT(m_def, "Structure def is nullptr.");
        m_effect->setPosition(pos + rotateAsMe(m_def->getEffectOffset()));
    }

    turret_setInWorldPosition(pos);
}

void Structure::setInWorldRotation(const engine::FloatVec3 &rot)
{
    base::setInWorldRotation(rot);

    if(m_model)
        m_model->setRotation(rot);

    if(m_rigidBody)
        m_rigidBody->setRotation(rot);

    if(m_effect) {
        E_DASSERT(m_def, "Structure def is nullptr.");
        m_effect->setRotation(rot);
    }

    turret_setInWorldRotation(rot);
}

bool Structure::canBuildOnTopOfIt() const
{
    return true;
}

bool Structure::wantsToBeRemovedFromWorld() const
{
    return isKilled();
}

std::vector <std::pair <engine::FloatVec3, engine::FloatVec3>> Structure::trySnapToMe(const StructureDef &structureDef, const engine::FloatVec3 &designatedPos) const
{
    TRACK;

    E_DASSERT(m_def, "Structure def is nullptr.");

    if(!m_def->getCanSnapToOtherStructures() || !structureDef.getCanSnapToOtherStructures())
        return {};

    return Snapper::trySnap(*this, structureDef, designatedPos);
}

bool Structure::canBeDeconstructed(const Character &doer) const
{
    return &getFactionDef() == &doer.getFactionDef();
}

bool Structure::isKilled() const
{
    return m_HP <= 0;
}

engine::FloatVec3 Structure::getAIAimPosition() const
{
    return getCenterPosition();
}

int Structure::getAIPotentialTargetPriority() const
{
    E_DASSERT(m_def, "Structure def is nullptr.");
    return m_def->getTurretInfo().isTurret() ? 2 : 1;
}

std::shared_ptr <EffectDef> Structure::getOnHitEffectDefPtr() const
{
    E_DASSERT(m_def, "Structure def is nullptr.");
    return m_def->getOnHitEffectDefPtr();
}

bool Structure::hasSearchableItemContainer() const
{
    E_DASSERT(m_def, "Structure def is nullptr.");
    return m_def->hasSearchableItemContainer();
}

std::shared_ptr <MultiSlotItemContainer> Structure::getSearchableItemContainer() const
{
    if(!m_searchableItemContainer)
        throw engine::Exception{"This structure does not have any searchable item container. This should have been checked before."};

    return m_searchableItemContainer;
}

std::string Structure::getName() const
{
    E_DASSERT(m_def, "Structure def is nullptr.");
    return m_def->getCapitalizedLabel();
}

void Structure::onInWorldUpdate()
{
    TRACK;

    E_DASSERT(m_def, "Structure def is nullptr.");
    E_DASSERT(m_model, "Model is nullptr.");
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    // update entitiy position and model position with physical body position

    const auto &pos = m_rigidBody->getPosition();
    const auto &rot = m_rigidBody->getRotation();

    base::setInWorldPosition(pos);
    base::setInWorldRotation(rot);

    m_model->setPosition(pos);
    m_model->setRotation(rot);

    if(m_effect) {
        E_DASSERT(m_def, "Structure def is nullptr.");
        m_effect->setPosition(pos + rotateAsMe(m_def->getEffectOffset()));
        m_effect->setRotation(rot);
    }

    if(m_def->hasMass())
        m_rigidBody->affectByWater(WorldPart::k_waterHeight);

    turret_onInWorldUpdate(pos, rot);
}

void Structure::onSpawnedInWorld()
{
    TRACK;

    base::onSpawnedInWorld();

    auto &device = Global::getCore().getDevice();
    const auto &pos = getInWorldPosition();
    const auto &rot = getInWorldRotation();

    E_DASSERT(m_def, "Structure def is nullptr.");

    m_model = device.getSceneManager().addModel(m_def->getModelDefPtr());
    m_model->setPosition(pos);
    m_model->setRotation(rot);

    const auto &cachedCollisionShapeDef = m_def->getCachedCollisionShapeDef();
    const auto &shape = cachedCollisionShapeDef.getCollisionShapePtr();
    const auto &posOffset = cachedCollisionShapeDef.getPosOffset();

    m_rigidBody = device.getPhysicsManager().addRigidBody(shape, m_def->getMass(), getEntityID(), posOffset);
    m_rigidBody->setPosition(pos);
    m_rigidBody->setRotation(rot);

    if(m_def->hasEffectDef())
        m_effect = std::make_unique <Effect> (m_def->getEffectDefPtr(), pos + rotateAsMe(m_def->getEffectOffset()), rot);

    m_shouldExplodeWhenRemovedFromWorld = false;

    turret_onSpawnedInWorld(pos, rot);
}

void Structure::onRemovedFromWorld()
{
    base::onRemovedFromWorld();

    E_DASSERT(m_def, "Mineable def is nullptr.");

    auto &core = Global::getCore();
    const auto &cachedCollisionShapeDef = m_def->getCachedCollisionShapeDef();
    auto &defsCache = core.getDefsCache();

    if(m_shouldExplodeWhenRemovedFromWorld) {
        auto &effectsPool = core.getEffectsPool();
        const auto &turretInfo = m_def->getTurretInfo();

        engine::FloatVec3 offset;

        if(turretInfo.isTurret())
            offset = {0.f, turretInfo.getDistanceToHead(), 0.f};
        else
            offset = cachedCollisionShapeDef.getPosOffset();

        effectsPool.add(defsCache.Effect_Explosion, getInWorldPosition() + offset, getInWorldRotation());
    }
    else {
        cachedCollisionShapeDef.addPoofEffect(getInWorldPosition(), getInWorldRotation());

        auto &soundPool = core.getSoundPool();
        soundPool.play(defsCache.Sound_Poof, getInWorldPosition());
    }

    m_model.reset();
    m_rigidBody.reset();
    m_effect.reset();

    turret_onRemovedFromWorld();
}

void Structure::onPointedByPlayer()
{
    if(m_model)
        m_model->highlightNextFrame();

    if(m_electricityComponent && m_electricityComponent->isInAnyElectricitySystem())
        m_electricityComponent->getElectricitySystem().setOverlay(ElectricitySystem::Overlay::NormalSymbolsAndLines);
}

void Structure::onDraw2DInfoWhenPointed()
{
    auto &core = Global::getCore();
    auto &device = core.getDevice();
    const auto &screenCenter = device.getScreenSize() / 2;
    const auto &GUIRenderer = device.getGUIManager().getRenderer();
    const auto &thisPlayerCharacter = core.getThisPlayer().getCharacter();
    auto pos = screenCenter.moved(35, -20);

    E_DASSERT(m_def, "Structure def is nullptr.");

    GUIRenderer.drawText(m_def->getCapitalizedLabel(), pos, {0.5f, 0.5f, 0.5f, 0.8f}, engine::GUI::IGUIRenderer::FontSize::Big);
    pos.y += GUIRenderer.getTextSize(m_def->getCapitalizedLabel(), engine::GUI::IGUIRenderer::FontSize::Big).y;

    const auto &hpStr = std::to_string(m_HP) + " / " + std::to_string(m_def->getMaxHP());
    GUIRenderer.drawText(hpStr, pos, {0.7f, 0.2f, 0.2f, 0.8f}, engine::GUI::IGUIRenderer::FontSize::Medium);
    pos.y += GUIRenderer.getTextSize(hpStr, engine::GUI::IGUIRenderer::FontSize::Medium).y;

    if(m_electricityComponent && m_electricityComponent->isInAnyElectricitySystem()) {
        engine::Color color{0.2f, 0.8f, 0.2f, 0.8f};

        if(!m_electricityComponent->isWorking())
            color = {0.8f, 0.2f, 0.2f, 0.8f};

        const auto &electricitySystem = m_electricityComponent->getElectricitySystem();

        const auto &requiredPowerStr = "Total required power: " + std::to_string(electricitySystem.getRequiredPower());
        const auto &generatedPowerStr = "Total generated power: " + std::to_string(electricitySystem.getGeneratedPower());

        GUIRenderer.drawText(requiredPowerStr, pos, color, engine::GUI::IGUIRenderer::FontSize::Medium);
        pos.y += GUIRenderer.getTextSize(requiredPowerStr, engine::GUI::IGUIRenderer::FontSize::Medium).y;

        GUIRenderer.drawText(generatedPowerStr, pos, color, engine::GUI::IGUIRenderer::FontSize::Medium);
        pos.y += GUIRenderer.getTextSize(generatedPowerStr, engine::GUI::IGUIRenderer::FontSize::Medium).y;
    }

    if(m_def->isWorkbench()) {
        std::string text = "Press E to use this workbench.";

        GUIRenderer.drawText(text, pos, {0.5f, 0.5f, 0.5f, 0.8f}, engine::GUI::IGUIRenderer::FontSize::Medium);
        pos.y += GUIRenderer.getTextSize(text, engine::GUI::IGUIRenderer::FontSize::Medium).y;
    }

    if(hasSearchableItemContainer()) {
        std::string text = "Press E to search.";

        GUIRenderer.drawText(text, pos, {0.5f, 0.5f, 0.5f, 0.8f}, engine::GUI::IGUIRenderer::FontSize::Medium);
        pos.y += GUIRenderer.getTextSize(text, engine::GUI::IGUIRenderer::FontSize::Medium).y;
    }

    if(canBeDeconstructed(thisPlayerCharacter))
        GUIRenderer.drawText("Hold X to deconstruct.", pos, {0.5f, 0.5f, 0.5f, 0.8f}, engine::GUI::IGUIRenderer::FontSize::Medium);
}

void Structure::onItemUsedOnMe(Entity &doer, const Item &item)
{
    if(isKilled())
        return;

    m_HP -= CharacterStatsOrSkillsRelatedFormulas::getDamage(doer, *this, item);

    if(m_HP <= 0) {
        m_HP = 0;

        E_DASSERT(m_def, "Structure def is nullptr.");

        if(m_def->shouldExplodeWhenDestroyed())
            m_shouldExplodeWhenRemovedFromWorld = true;
    }
}

void Structure::onDeconstructed(Character &doer)
{
    TRACK;

    E_DASSERT(m_def, "Structure def is nullptr.");

    auto &inv = doer.getInventory().getMultiSlotItemContainer();
    auto &world = Global::getCore().getWorld();

    for(const auto &elem : m_def->getItemsWhenDeconstructed().getItems()) {
        const auto &item = std::make_shared <Item> (world.getUniqueEntityID(), elem.getItemDefPtr(), elem.getStack());
        inv.tryAddItem(item);
    }
}

void Structure::onCharacterStepOnIt(Character &character)
{
    E_DASSERT(m_def, "Structure def is nullptr.");

    if(m_def->hasStepSound())
        Global::getCore().getSoundPool().play(m_def->getStepSoundDefPtr(), character.getInWorldPosition());
}

engine::FloatVec3 Structure::getCenterPosition() const
{
    E_DASSERT(m_def, "Structure def is nullptr.");

    // if we add pos offset to our current position we will get the center of physical body shape

    return getInWorldPosition() + rotateAsMe(m_def->getCachedCollisionShapeDef().getPosOffset());
}

const engine::FloatVec3 &Structure::getTurretHeadRotation() const
{
    return m_turretHeadRot;
}

void Structure::setTurretHeadRotation(const engine::FloatVec3 &turretHeadRot)
{
    m_turretHeadRot = turretHeadRot;

    if(m_turretHeadModel)
        m_turretHeadModel->setRotation(m_turretHeadRot);

    if(m_turretHeadRigidBody)
        m_turretHeadRigidBody->setRotation(m_turretHeadRot);
}

ElectricityComponent &Structure::getElectricityComponent() const
{
    if(!m_electricityComponent)
        throw engine::Exception{"Electricity component is nullptr."};

    return *m_electricityComponent;
}

StructureDef &Structure::getDef() const
{
    E_DASSERT(m_def, "Structure def is nullptr.");

    return *m_def;
}

void Structure::setOwner(const Character &character)
{
    m_ownerEntityID = character.getEntityID();
}

bool Structure::hasOwner() const
{
    auto &world = Global::getCore().getWorld();

    return m_ownerEntityID >= 0 && world.entityExists(m_ownerEntityID) && !world.getEntity(m_ownerEntityID).isKilled();
}

Character &Structure::getOwner() const
{
    if(m_ownerEntityID < 0)
        throw engine::Exception{"Tried to get nullptr owner."};

    auto &ownerCharacter = Global::getCore().getWorld().getEntityAndCast <Character> (m_ownerEntityID);

    return ownerCharacter;
}

bool Structure::rayTest_notTurretHead(const engine::FloatVec3 &start, const engine::FloatVec3 &end, engine::app3D::CollisionFilter withWhatCollide, engine::FloatVec3 &outPos, int &outHitBodyUserIndex) const
{
    TRACK;

    auto &physicsManager = Global::getCore().getDevice().getPhysicsManager();
    return physicsManager.rayTest_notMe(start, end, m_turretHeadRigidBody, withWhatCollide, outPos, outHitBodyUserIndex);
}

Structure::~Structure() = default;

void Structure::turret_setInWorldPosition(const engine::FloatVec3 &pos)
{
    E_DASSERT(m_def, "Structure def is nullptr.");

    float offset{m_def->getTurretInfo().getDistanceToHead()};

    if(m_turretHeadModel)
        m_turretHeadModel->setPosition(pos.movedY(offset));

    if(m_turretHeadRigidBody)
        m_turretHeadRigidBody->setPosition(pos.movedY(offset));
}

void Structure::turret_setInWorldRotation(const engine::FloatVec3 &rot)
{
}

void Structure::turret_onInWorldUpdate(const engine::FloatVec3 &rigidBodyPos, const engine::FloatVec3 &rigidBodyRot)
{
    float offset{m_def->getTurretInfo().getDistanceToHead()};

    if(m_turretHeadModel)
        m_turretHeadModel->setPosition(rigidBodyPos.movedY(offset));

    if(m_turretHeadRigidBody)
        m_turretHeadRigidBody->setPosition(rigidBodyPos.movedY(offset));

    if(m_turretComponent)
        m_turretComponent->onInWorldUpdate();
}

void Structure::turret_onSpawnedInWorld(const engine::FloatVec3 &inWorldPos, const engine::FloatVec3 &inWorldRot)
{
    E_DASSERT(m_def, "Structure def is nullptr.");

    m_turretHeadRot = inWorldRot;

    const auto &turretInfo = m_def->getTurretInfo();

    if(!turretInfo.isTurret())
        return;

    auto &device = Global::getCore().getDevice();

    float offset{m_def->getTurretInfo().getDistanceToHead()};

    m_turretHeadModel = device.getSceneManager().addModel(turretInfo.getHeadModelDefPtr());
    m_turretHeadModel->setPosition(inWorldPos.movedY(offset));
    m_turretHeadModel->setRotation(m_turretHeadRot);

    const auto &cachedCollisionShapeDef = turretInfo.getHeadCachedCollisionShapeDef();
    const auto &shape = cachedCollisionShapeDef.getCollisionShapePtr();
    const auto &posOffset = cachedCollisionShapeDef.getPosOffset();

    m_turretHeadRigidBody = device.getPhysicsManager().addRigidBody(shape, 0.f, getEntityID(), posOffset);
    m_turretHeadRigidBody->setPosition(inWorldPos.movedY(offset));
    m_turretHeadRigidBody->setRotation(m_turretHeadRot);
}

void Structure::turret_onRemovedFromWorld()
{
    if(m_turretHeadRigidBody && !m_shouldExplodeWhenRemovedFromWorld) {
        E_DASSERT(m_def, "Mineable def is nullptr.");

        const auto &turretInfo = m_def->getTurretInfo();
        const auto &cachedCollisionShapeDef = turretInfo.getHeadCachedCollisionShapeDef();

        cachedCollisionShapeDef.addPoofEffect(getInWorldPosition().movedY(turretInfo.getDistanceToHead()), getTurretHeadRotation());
    }

    m_turretHeadModel.reset();
    m_turretHeadRigidBody.reset();
}

const engine::IntVec2 Structure::k_searchableItemContainerSize{6, 6};

} // namespace app
