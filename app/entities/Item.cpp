#include "Item.hpp"

#include "engine/app3D/sceneNodes/Model.hpp"
#include "engine/app3D/physics/RigidBody.hpp"
#include "engine/app3D/physics/CollisionDetector.hpp"
#include "engine/app3D/managers/SceneManager.hpp"
#include "engine/app3D/managers/PhysicsManager.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/GUI/GUIManager.hpp"
#include "engine/GUI/IGUIRenderer.hpp"
#include "../GUI/MainGUI.hpp"
#include "../defs/DefsCache.hpp"
#include "../defs/ItemDef.hpp"
#include "../defs/CachedCollisionShapeDef.hpp"
#include "../defs/FactionDef.hpp"
#include "../defs/FactionRelationDef.hpp"
#include "../world/World.hpp"
#include "../world/WorldPart.hpp"
#include "../thisPlayer/ThisPlayer.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "../SoundPool.hpp"
#include "../EffectsPool.hpp"
#include "Character.hpp"

namespace app
{

Item::Item(int entityID, const std::shared_ptr <ItemDef> &def, int stack)
    : Entity{entityID},
      m_stack{stack},
      m_def{def},
      m_isPickedUp{}
{
    TRACK;

    if(!m_def)
        throw engine::Exception{"Item def is nullptr."};

    if(m_stack <= 0)
        throw engine::Exception{"Tried to create Item with <= 0 stack."};

    if(m_stack > m_def->getMaxStack()) {
        m_stack = m_def->getMaxStack();
        E_WARNING("Tried to create Item with stack greater than max stack. Fixed.");
    }
}

bool Item::wantsEverInWorldUpdate() const
{
    E_DASSERT(m_def, "Item def is nullptr.");

    return m_def->hasMass();
}

void Item::setInWorldPosition(const engine::FloatVec3 &pos)
{
    base::setInWorldPosition(pos);

    if(m_model)
        m_model->setPosition(pos);

    if(m_rigidBody)
        m_rigidBody->setPosition(pos);

    if(m_collisionDetector)
        m_collisionDetector->setPosition(pos);
}

void Item::setInWorldRotation(const engine::FloatVec3 &rot)
{
    base::setInWorldRotation(rot);

    if(m_model)
        m_model->setRotation(rot);

    if(m_rigidBody)
        m_rigidBody->setRotation(rot);

    if(m_collisionDetector)
        m_collisionDetector->setRotation(rot);
}

bool Item::wantsToBeRemovedFromWorld() const
{
    return !m_stack || m_isPickedUp;
}

std::string Item::getName() const
{
    E_DASSERT(m_def, "Item def is nullptr.");
    return m_def->getCapitalizedLabel();
}

void Item::onInWorldUpdate()
{
    TRACK;

    E_DASSERT(m_def, "Item def is nullptr.");
    E_DASSERT(m_model, "Model is nullptr.");
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");
    E_DASSERT(m_collisionDetector, "Collision detector is nullptr.");

    // update entitiy position and model position with physical body position

    const auto &pos = m_rigidBody->getPosition();
    const auto &rot = m_rigidBody->getRotation();

    base::setInWorldPosition(pos);
    base::setInWorldRotation(rot);

    m_model->setPosition(pos);
    m_model->setRotation(rot);

    m_collisionDetector->setPosition(pos);
    m_collisionDetector->setRotation(rot);

    if(m_def->hasMass())
        m_rigidBody->affectByWater(WorldPart::k_waterHeight);

    auto &world = Global::getCore().getWorld();

    for(auto entityID : m_collisionDetector->getCollidedObjectsUserIndices()) {
        if(world.entityExists(entityID)) {
            if(world.getEntity(entityID).tryPickUpItem(world.getEntityPtrAndCast <Item> (getEntityID()))) {
                m_isPickedUp = true;
                // wantsToBeRemovedFromWorld() will be called right after returning from this method,
                // so there is no point in checking or using m_isPickedUp in other methods
                break;
            }

            if(!m_stack)
                break;
        }
    }
}

void Item::onSpawnedInWorld()
{
    TRACK;

    base::onSpawnedInWorld();

    auto &core = Global::getCore();
    auto &device = core.getDevice();
    const auto &pos = getInWorldPosition();
    const auto &rot = getInWorldRotation();

    E_DASSERT(m_def, "Item def is nullptr.");

    m_model = device.getSceneManager().addModel(m_def->getModelDefPtr());
    m_model->setPosition(pos);
    m_model->setRotation(rot);

    const auto &cachedCollisionShapeDef = m_def->getCachedCollisionShapeDef();
    const auto &shape = cachedCollisionShapeDef.getCollisionShapePtr();
    const auto &posOffset = cachedCollisionShapeDef.getPosOffset();

    m_rigidBody = device.getPhysicsManager().addRigidBody(shape, m_def->getMass(), getEntityID(), posOffset);
    m_rigidBody->setPosition(pos);
    m_rigidBody->setRotation(rot);
    m_rigidBody->setFriction(0.5f, 0.1f);

    const auto &defsCache = core.getDefsCache();

    m_collisionDetector = device.getPhysicsManager().addCollisionDetector(
        defsCache.CachedCollisionShape_ItemPickUpZone->getCollisionShapePtr(),
        engine::app3D::CollisionFilter::GhostMisc,
        engine::app3D::CollisionFilter::Character | engine::app3D::CollisionFilter::CharacterHitBox,
        {});

    m_collisionDetector->setPosition(pos);
    m_collisionDetector->setRotation(rot);

    m_isPickedUp = false;
}

void Item::onRemovedFromWorld()
{
    base::onRemovedFromWorld();

    m_model.reset();
    m_rigidBody.reset();
    m_collisionDetector.reset();
}

void Item::onUsedOnSomething(const engine::FloatVec3 &sourcePos, const engine::FloatVec3 &hitPos, int hitEntityID, Entity &doer) const
{
    E_DASSERT(m_def, "Item def is nullptr.");

    const auto &onUsed = m_def->getOnUsed();
    auto &core = Global::getCore();
    auto &mainGUI = core.getMainGUI();
    auto &soundPool = core.getSoundPool();
    auto &effectsPool = core.getEffectsPool();
    auto &defsCache = core.getDefsCache();

    auto effectDef = defsCache.Effect_OnHitGeneric;

    if(hitEntityID >= 0) {
        auto &world = core.getWorld();
        auto entityPtr = world.getEntityPtr(hitEntityID);

        E_DASSERT(entityPtr, "Entity is nullptr.");

        effectDef = entityPtr->getOnHitEffectDefPtr();
        E_DASSERT(effectDef, "Effect def returned by Entity is nullptr.");

        const auto &thisPlayerCharacter = core.getThisPlayer().getCharacter();

        if(&doer == &thisPlayerCharacter &&
           !entityPtr->isKilled() &&
           entityPtr->getFactionDef().getRelation(doer.getFactionDef()) == FactionRelationDef::Relation::Hostile) {
            soundPool.play(defsCache.Sound_HitTarget);
            mainGUI.onHitTarget();
        }

        entityPtr->onItemUsedOnMe(doer, *this);

        if(entityPtr->wantsToBeRemovedFromWorld())
            world.removeEntity(hitEntityID);
    }

    effectsPool.add(effectDef, hitPos);
}

const ItemDef &Item::getDef() const
{
    E_DASSERT(m_def, "Item def is nullptr.");

    return *m_def;
}

const std::shared_ptr <ItemDef> &Item::getDefPtr() const
{
    E_DASSERT(m_def, "Item def is nullptr.");

    return m_def;
}

int Item::getStack() const
{
    return m_stack;
}

bool Item::hasFullStack() const
{
    E_DASSERT(m_def, "Item def is nullptr.");

    return m_stack == m_def->getMaxStack();
}

std::shared_ptr <Item> Item::split(int getThisMany)
{
    TRACK;

    if(getThisMany <= 0)
        throw engine::Exception{"Tried to get <= 0 stack by splitting."};

    if(getThisMany >= m_stack)
        throw engine::Exception{"Tried to get >= m_stack stack by splitting."};

    auto &world = Global::getCore().getWorld();

    const auto &ret = std::make_shared <Item> (world.getUniqueEntityID(), m_def, getThisMany);

    m_stack -= getThisMany;

    return ret;
}

int Item::tryMergeStack(Item &mergeWithThis, int tryTakeThisMany)
{
    TRACK;

    if(this == &mergeWithThis)
        throw engine::Exception{"Tried to merge stacks of the exact same objects."};

    if(m_def != mergeWithThis.getDefPtr())
        return 0;

    tryTakeThisMany = std::min({tryTakeThisMany, m_def->getMaxStack() - m_stack, mergeWithThis.getStack()});

    if(tryTakeThisMany <= 0)
        return 0;

    // note: it's the callers responsibility to remove Item with stack == 0, after returning from this method

    m_stack += tryTakeThisMany;
    mergeWithThis.m_stack -= tryTakeThisMany;

    return tryTakeThisMany;
}

void Item::onPointedByPlayer()
{
    if(m_model)
        m_model->highlightNextFrame();
}

void Item::onDraw2DInfoWhenPointed()
{
    auto &device = Global::getCore().getDevice();
    const auto &screenCenter = device.getScreenSize() / 2;
    const auto &GUIRenderer = device.getGUIManager().getRenderer();
    auto pos = screenCenter.moved(35, -20);

    E_DASSERT(m_def, "Item def is nullptr.");

    GUIRenderer.drawText(m_def->getCapitalizedLabel(), pos, {0.5f, 0.5f, 0.5f, 0.8f}, engine::GUI::IGUIRenderer::FontSize::Big);
    pos.y += GUIRenderer.getTextSize(m_def->getCapitalizedLabel(), engine::GUI::IGUIRenderer::FontSize::Big).y;

    GUIRenderer.drawText("Press E to pick up.", pos, {0.5f, 0.5f, 0.5f, 0.8f}, engine::GUI::IGUIRenderer::FontSize::Medium);
}

} // namespace app
