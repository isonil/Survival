#include "Mineable.hpp"

#include "engine/app3D/sceneNodes/Model.hpp"
#include "engine/app3D/physics/RigidBody.hpp"
#include "engine/app3D/managers/SceneManager.hpp"
#include "engine/app3D/managers/PhysicsManager.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/GUI/GUIManager.hpp"
#include "engine/GUI/IGUIRenderer.hpp"
#include "engine/util/StringUtility.hpp"
#include "../defs/DefsCache.hpp"
#include "../defs/MineableDef.hpp"
#include "../defs/ItemDef.hpp"
#include "../defs/CachedCollisionShapeDef.hpp"
#include "../world/World.hpp"
#include "../world/WorldPart.hpp"
#include "../itemContainers/MultiSlotItemContainer.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "../SoundPool.hpp"
#include "Item.hpp"
#include "Character.hpp"

namespace app
{

ResourceInMineable::ResourceInMineable(const std::shared_ptr <ItemDef> &itemDef, int count)
    : m_count{count},
      m_itemDef{itemDef}
{
    if(!m_itemDef)
        throw engine::Exception{"Item def is nullptr."};

    if(m_count < 0)
        throw engine::Exception{"Count can't be negative."};
}

void ResourceInMineable::subtract(int count)
{
    m_count -= count;

    if(m_count < 0)
        m_count = 0;
}

const std::shared_ptr <ItemDef> &ResourceInMineable::getItemDefPtr() const
{
    E_DASSERT(m_itemDef, "Item def is nullptr.");

    return m_itemDef;
}

ItemDef &ResourceInMineable::getItemDef() const
{
    E_DASSERT(m_itemDef, "Item def is nullptr.");

    return *m_itemDef;
}

int ResourceInMineable::getCount() const
{
    return m_count;
}

Mineable::Mineable(int entityID, const std::shared_ptr <MineableDef> &def)
    : Entity{entityID},
      m_def{def},
      m_durability{}
{
    TRACK;

    if(!m_def)
        throw engine::Exception{"Mineable def is nullptr."};

    m_durability = m_def->getInitialDurability();

    initResources();
}

bool Mineable::wantsEverInWorldUpdate() const
{
    E_DASSERT(m_def, "Mineable def is nullptr.");

    return m_def->hasMass();
}

void Mineable::setInWorldPosition(const engine::FloatVec3 &pos)
{
    base::setInWorldPosition(pos);

    if(m_model)
        m_model->setPosition(pos);

    if(m_rigidBody)
        m_rigidBody->setPosition(pos);
}

void Mineable::setInWorldRotation(const engine::FloatVec3 &rot)
{
    base::setInWorldRotation(rot);

    if(m_model)
        m_model->setRotation(rot);

    if(m_rigidBody)
        m_rigidBody->setRotation(rot);
}

bool Mineable::wantsToBeRemovedFromWorld() const
{
    return m_durability <= 0 && !hasAnyResources();
}

bool Mineable::blocksWorldPartFreePosFinderField() const
{
    return true;
}

std::shared_ptr <EffectDef> Mineable::getOnHitEffectDefPtr() const
{
    E_DASSERT(m_def, "Mineable def is nullptr.");
    return m_def->getOnHitEffectDefPtr();
}

std::string Mineable::getName() const
{
    E_DASSERT(m_def, "Mineable def is nullptr.");
    return m_def->getCapitalizedLabel();
}

void Mineable::onInWorldUpdate()
{
    TRACK;

    E_DASSERT(m_def, "Mineable def is nullptr.");
    E_DASSERT(m_model, "Model is nullptr.");
    E_DASSERT(m_rigidBody, "Rigid body is nullptr.");

    // update entitiy position and model position with physical body position

    const auto &pos = m_rigidBody->getPosition();
    const auto &rot = m_rigidBody->getRotation();

    base::setInWorldPosition(pos);
    base::setInWorldRotation(rot);

    m_model->setPosition(pos);
    m_model->setRotation(rot);

    if(m_def->hasMass())
        m_rigidBody->affectByWater(WorldPart::k_waterHeight);
}

void Mineable::onSpawnedInWorld()
{
    TRACK;

    base::onSpawnedInWorld();

    auto &device = Global::getCore().getDevice();
    const auto &pos = getInWorldPosition();
    const auto &rot = getInWorldRotation();

    E_DASSERT(m_def, "Mineable def is nullptr.");

    m_model = device.getSceneManager().addModel(m_def->getRandomModelDefPtr());
    m_model->setPosition(pos);
    m_model->setRotation(rot);

    const auto &cachedCollisionShapeDef = m_def->getCachedCollisionShapeDef();
    const auto &shape = cachedCollisionShapeDef.getCollisionShapePtr();
    const auto &posOffset = cachedCollisionShapeDef.getPosOffset();

    m_rigidBody = device.getPhysicsManager().addRigidBody(shape, m_def->getMass(), getEntityID(), posOffset);
    m_rigidBody->setPosition(pos);
    m_rigidBody->setRotation(rot);
}

void Mineable::onRemovedFromWorld()
{
    base::onRemovedFromWorld();

    E_DASSERT(m_def, "Mineable def is nullptr.");

    auto &core = Global::getCore();

    if(m_rigidBody) {
        const auto &cachedCollisionShapeDef = m_def->getCachedCollisionShapeDef();
        cachedCollisionShapeDef.addPoofEffect(getInWorldPosition(), getInWorldRotation());

        auto &defsCache = core.getDefsCache();
        auto &soundPool = core.getSoundPool();

        soundPool.play(defsCache.Sound_Poof, getInWorldPosition());
    }

    const auto &toSpawn = m_def->getItemsToSpawnWhenDestroyed();
    const auto &posToSpawn = getInWorldPosition() + rotateAsMe(m_def->getCachedCollisionShapeDef().getPosOffset());
    auto &world = core.getWorld();

    for(const auto &elem : toSpawn.getItems()) {
        const auto &item = std::make_shared <Item> (world.getUniqueEntityID(), elem.getItemDefPtr(), elem.getStack());
        item->setInWorldPosition(posToSpawn);
        world.addEntity(item);
    }

    m_model.reset();
    m_rigidBody.reset();
}

void Mineable::onPointedByPlayer()
{
    if(m_model)
        m_model->highlightNextFrame();
}

void Mineable::onDraw2DInfoWhenPointed()
{
    TRACK;

    auto &device = Global::getCore().getDevice();
    const auto &screenCenter = device.getScreenSize() / 2;
    const auto &GUIRenderer = device.getGUIManager().getRenderer();
    auto pos = screenCenter.moved(35, -20);

    E_DASSERT(m_def, "Mineable def is nullptr.");

    GUIRenderer.drawText(m_def->getCapitalizedLabel(), pos, {0.5f, 0.5f, 0.5f, 0.8f}, engine::GUI::IGUIRenderer::FontSize::Big);

    pos.y += GUIRenderer.getTextSize(m_def->getCapitalizedLabel(), engine::GUI::IGUIRenderer::FontSize::Big).y;

    if(m_durability) {
        const auto &str = "Durability: " + std::to_string(m_durability) + '%';

        GUIRenderer.drawText(str, pos, {0.5f, 0.5f, 0.8f, 0.8f}, engine::GUI::IGUIRenderer::FontSize::Medium);
        pos.y += GUIRenderer.getTextSize(str, engine::GUI::IGUIRenderer::FontSize::Medium).y;
    }

    for(const auto &elem : m_resources) {
        int count{elem.getCount()};

        if(count <= 0)
            continue;

        const auto &str = elem.getItemDef().getLabel() + " x" + std::to_string(count);
        GUIRenderer.drawText(str, pos, {0.5f, 0.5f, 0.8f, 0.8f}, engine::GUI::IGUIRenderer::FontSize::Medium);

        pos.y += GUIRenderer.getTextSize(str, engine::GUI::IGUIRenderer::FontSize::Medium).y;
    }
}

void Mineable::onItemUsedOnMe(Entity &doer, const Item &item)
{
    TRACK;

    auto *characterDoer = dynamic_cast <Character*> (&doer);

    if(!characterDoer)
        return;

    const auto &gathersResourcesProperties = item.getDef().getOnUsed().getGathersResourcesProperties();

    bool foundTag{};

    E_DASSERT(m_def, "Mineable def is nullptr.");

    for(const auto &elem : gathersResourcesProperties.getMineableTags()) {
        if(m_def->hasMineableTag(elem)) {
            foundTag = true;
            break;
        }
    }

    if(!foundTag)
        return;

    m_durability -= gathersResourcesProperties.getDecreaseDurabilityBy();
    m_durability = std::max(0, m_durability);

    // now, we'll gather resources

    if(!gathersResourcesProperties.gathersAnything())
        return;

    std::vector <int> candidates;

    for(size_t i = 0; i < m_resources.size(); ++i) {
        if(m_resources[i].getCount() > 0)
            candidates.push_back(i);
    }

    if(candidates.empty())
        return;

    int candidate{engine::Random::rangeExclusive(0, candidates.size())};

    auto &resource = m_resources[candidates[candidate]];

    int desiredGatheredCount{gathersResourcesProperties.getGatheredCount()};

    int countToGather{std::min({desiredGatheredCount, resource.getCount(), resource.getItemDef().getMaxStack()})};

    if(countToGather <= 0)
        return;

    auto &world = Global::getCore().getWorld();

    const auto &gatheredItem = std::make_shared <Item> (world.getUniqueEntityID(), resource.getItemDefPtr(), countToGather);

    if(int added = characterDoer->getInventory().getMultiSlotItemContainer().tryAddItem(gatheredItem))
        resource.subtract(added);
}

bool Mineable::hasAnyResources() const
{
    return std::any_of(m_resources.begin(), m_resources.end(), [](const auto &resource) {
        return resource.getCount() > 0;
    });
}

const std::vector <ResourceInMineable> &Mineable::getResources() const
{
    return m_resources;
}

void Mineable::initResources()
{
    TRACK;

    E_DASSERT(m_def, "Mineable def is nullptr.");

    const auto &defResources = m_def->getResources();

    m_resources.clear();
    m_resources.reserve(defResources.getItems().size());

    for(const auto &elem : defResources.getItems()) {
        m_resources.emplace_back(elem.getItemDefPtr(), elem.getCount());
    }
}

} // namespace app
