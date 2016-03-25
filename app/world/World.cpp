#include "World.hpp"

#include "../entities/components/ElectricityComponent.hpp"
#include "engine/app3D/managers/PhysicsManager.hpp"
#include "engine/app3D/managers/ResourcesManager.hpp"
#include "engine/app3D/physics/RigidBody.hpp"
#include "engine/app3D/physics/StaticPlaneShape.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/app3D/Settings.hpp"
#include "engine/util/DefDatabase.hpp"
#include "engine/util/DataFile.hpp"
#include "../defs/DefsCache.hpp"
#include "../defs/CachedCollisionShapeDef.hpp"
#include "../defs/StructureDef.hpp"
#include "../defs/WorldPartDef.hpp"
#include "../entities/Entity.hpp"
#include "../entities/Structure.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "WorldPart.hpp"
#include "ElectricitySystem.hpp"

namespace app
{

struct WorldPartsParams : engine::DataFile::Saveable
{
    void expose(engine::DataFile::Node &node) override;

    std::vector <WorldPart> worldParts;
};

void WorldPartsParams::expose(engine::DataFile::Node &node)
{
    node.var(worldParts, "list");
}

World::World(const engine::app3D::Settings &settings)
    : m_uniqueEntityID{},
      m_birdsAmbience{Global::getCore().getDevice().getResourcesManager().getPathToResource(k_birdsAmbiencePath)}
{
    TRACK;

    auto &core = Global::getCore();
    auto &device = core.getDevice();
    const auto &defsCache = core.getDefsCache();

    const auto &planeShape = defsCache.CachedCollisionShape_Plane_0->getCollisionShapePtr();
    m_staticInfinitePlane = device.getPhysicsManager().addRigidBody(planeShape, 0.f);

    auto foundPlayerStartingWorldPart = false;

    for(const auto &elem : settings.mods.mods) {
        if(!elem.enabled)
            continue;

        WorldPartsParams worldParts;
        engine::DataFile dataFile;

        if(!dataFile.open("mods/" + elem.path + "/WorldParts.yaml", false))
            continue;

        dataFile.load(worldParts, "worldParts");

        for(const auto &elem : worldParts.worldParts) {
            const auto &def = elem.getDef();

            if(def.isPlayerStartingWorldPart()) {
                const auto &tilePosition = elem.getTilePosition();

                foundPlayerStartingWorldPart = true;
                m_playerStartingPosition = def.getPlayerStartingPosition().moved(tilePosition.x * WorldPart::k_terrainSize, tilePosition.y * WorldPart::k_terrainSize);
            }

            m_worldParts.emplace_back(std::make_unique <WorldPart> (std::move(elem)));
        }
    }

    engine::IntVec2 minTilePos{2, 2};
    engine::IntVec2 maxTilePos{2, 2};
    std::vector <engine::IntVec2> occupiedPositions;
    auto first = true;

    for(const auto &elem : m_worldParts) {
        E_DASSERT(elem, "World part is nullptr.");

        const auto &tilePosition = elem->getTilePosition();

        if(first) {
            minTilePos = tilePosition;
            maxTilePos = tilePosition;
            first = false;
        }
        else {
            minTilePos.x = std::min(minTilePos.x, tilePosition.x);
            minTilePos.y = std::min(minTilePos.y, tilePosition.y);
            maxTilePos.x = std::max(maxTilePos.x, tilePosition.x);
            maxTilePos.y = std::max(maxTilePos.y, tilePosition.y);
        }

        occupiedPositions.push_back(tilePosition);
    }

    m_bounds.pos = {(minTilePos.x - 1) * WorldPart::k_terrainSize,
                    (minTilePos.y - 1) * WorldPart::k_terrainSize};

    m_bounds.size = {(maxTilePos.x - minTilePos.x + 3) * WorldPart::k_terrainSize,
                     (maxTilePos.y - minTilePos.y + 3) * WorldPart::k_terrainSize};

    if(!foundPlayerStartingWorldPart)
        m_playerStartingPosition = m_bounds.pos + m_bounds.size / 2.f;

    for(int x = minTilePos.x - 2; x <= maxTilePos.x + 2; ++x) {
        for(int y = minTilePos.y - 2; y <= maxTilePos.y + 2; ++y) {
            engine::IntVec2 tilePosition{x, y};

            if(std::find(occupiedPositions.begin(), occupiedPositions.end(), tilePosition) == occupiedPositions.end()) {
                m_worldParts.emplace_back(std::make_unique <WorldPart> ());
                m_worldParts.back()->makeItWaterWorldPart(tilePosition);
            }
        }
    }

    for(const auto &elem : m_worldParts) {
        E_DASSERT(elem, "World part is nullptr.");
        elem->generateEntities(*this);
    }

    m_birdsAmbience.play();
}

void World::update()
{
    TRACK;

    m_dateTimeManager.update();
    m_spawnManager.update();

    for(auto it = m_entities_wantUpdate.begin(); it != m_entities_wantUpdate.end();) {
        E_DASSERT(it->second, "Entity is nullptr.");

        if(it->second->wantsToBeRemovedFromWorld()) {
            it->second->onRemovedFromWorld();

            removeFromQuickAccessCachedEntities(*it->second);
            it = m_entities_wantUpdate.erase(it);
        }
        else {
            it->second->onInWorldUpdate();
            ++it;
        }
    }

    updateElectricitySystems();
}

void World::addEntity(const std::shared_ptr <Entity> &entity)
{
    if(!entity)
        throw engine::Exception{"Tried to add nullptr entity."};

    auto ID = entity->getEntityID();

    if(m_entities_wantUpdate.find(ID) == m_entities_wantUpdate.end() &&
       m_entities_dontWantUpdate.find(ID) == m_entities_dontWantUpdate.end()) {
        if(entity->wantsEverInWorldUpdate())
            m_entities_wantUpdate.emplace(ID, entity);
        else
            m_entities_dontWantUpdate.emplace(ID, entity);

        addToQuickAccessCachedEntities(entity);

        if(entity->blocksWorldPartFreePosFinderField())
            useWorldPartFreePosFinderFieldAt(entity->getInWorldPosition());

        entity->onSpawnedInWorld();
    }
    else {
        E_WARNING("Could not add entity with ID \"%d\" to the world because entity with the same ID is already present.",
                  static_cast <int> (ID));
    }
}

void World::removeEntity(int entityID)
{
    TRACK;

    const auto &it1 = m_entities_dontWantUpdate.find(entityID);

    if(it1 != m_entities_dontWantUpdate.end()) {
        E_DASSERT(it1->second, "Entity is nullptr.");

        if(it1->second->blocksWorldPartFreePosFinderField()) {
            auto *worldPart = getWorldPart(it1->second->getInWorldPosition());

            if(worldPart)
                worldPart->setFreePosFinderDirty();
        }

        it1->second->onRemovedFromWorld();

        removeFromQuickAccessCachedEntities(*it1->second);
        m_entities_dontWantUpdate.erase(it1);

        return;
    }

    const auto &it2 = m_entities_wantUpdate.find(entityID);

    if(it2 != m_entities_wantUpdate.end()) {
        E_DASSERT(it2->second, "Entity is nullptr.");

        if(it2->second->blocksWorldPartFreePosFinderField()) {
            auto *worldPart = getWorldPart(it2->second->getInWorldPosition());

            if(worldPart)
                worldPart->setFreePosFinderDirty();
        }

        it2->second->onRemovedFromWorld();

        removeFromQuickAccessCachedEntities(*it2->second);
        m_entities_wantUpdate.erase(it2);
    }
}

bool World::entityExists(int entityID) const
{
    const auto &it1 = m_entities_dontWantUpdate.find(entityID);

    if(it1 != m_entities_dontWantUpdate.end())
        return true;

    const auto &it2 = m_entities_wantUpdate.find(entityID);

    if(it2 != m_entities_wantUpdate.end())
        return true;

    return false;
}

Entity &World::getEntity(int entityID) const
{
    const auto &ptr = getEntityPtr(entityID);

    E_DASSERT(ptr, "Entity is nullptr.");

    return *ptr;
}

const std::shared_ptr <Entity> &World::getEntityPtr(int entityID) const
{
    const auto &it1 = m_entities_dontWantUpdate.find(entityID);

    if(it1 != m_entities_dontWantUpdate.end()) {
        E_DASSERT(it1->second, "Entity is nullptr.");
        return it1->second;
    }

    const auto &it2 = m_entities_wantUpdate.find(entityID);

    if(it2 != m_entities_wantUpdate.end()) {
        E_DASSERT(it2->second, "Entity is nullptr.");
        return it2->second;
    }

    throw engine::Exception{"Could not find entity with ID \"" + std::to_string(entityID) + "\"."};
}

int World::getUniqueEntityID()
{
    ++m_uniqueEntityID;
    return m_uniqueEntityID - 1;
}

WorldPart *World::getWorldPart(const engine::FloatVec2 &pos) const
{
    auto it = std::find_if(m_worldParts.begin(), m_worldParts.end(), [&pos](const auto &elem) {
        E_DASSERT(elem, "World part is nullptr.");

        const auto &tilePosition = elem->getTilePosition();

        return pos.x >= tilePosition.x * WorldPart::k_terrainSize &&
               pos.x <= (tilePosition.x + 1) * WorldPart::k_terrainSize &&
               pos.y >= tilePosition.y * WorldPart::k_terrainSize &&
               pos.y <= (tilePosition.y + 1) * WorldPart::k_terrainSize;
    });

    if(it != m_worldParts.end())
        return it->get();

    return nullptr;
}

WorldPart *World::getWorldPart(const engine::FloatVec3 &pos) const
{
    return getWorldPart({pos.x, pos.z});
}

float World::getHeight(const engine::FloatVec2 &pos) const
{
    auto *worldPart = getWorldPart(pos);

    if(worldPart) {
        const auto &tilePosition = worldPart->getTilePosition();

        return worldPart->getHeight(pos.moved(-tilePosition.x * WorldPart::k_terrainSize,
                                              -tilePosition.y * WorldPart::k_terrainSize));
    }

    return 0.f;
}

float World::getHeight(const engine::FloatVec3 &pos) const
{
    return getHeight({pos.x, pos.z});
}

float World::getSlope(const engine::FloatVec2 &pos) const
{
    auto *worldPart = getWorldPart(pos);

    if(worldPart) {
        const auto &tilePosition = worldPart->getTilePosition();

        return worldPart->getSlope(pos.moved(-tilePosition.x * WorldPart::k_terrainSize,
                                             -tilePosition.y * WorldPart::k_terrainSize));
    }

    return 0.f;
}

float World::getSlope(const engine::FloatVec3 &pos) const
{
    return getSlope({pos.x, pos.z});
}

GroundType World::getGroundType(const engine::FloatVec2 &pos) const
{
    auto *worldPart = getWorldPart(pos);

    if(worldPart) {
        const auto &tilePosition = worldPart->getTilePosition();

        return worldPart->getGroundType(pos.moved(-tilePosition.x * WorldPart::k_terrainSize,
                                                  -tilePosition.y * WorldPart::k_terrainSize));
    }

    return GroundType::UnderWater;
}

GroundType World::getGroundType(const engine::FloatVec3 &pos) const
{
    return getGroundType({pos.x, pos.z});
}

DateTimeManager &World::getDateTimeManager()
{
    return m_dateTimeManager;
}

const DateTimeManager &World::getDateTimeManager() const
{
    return m_dateTimeManager;
}

const engine::FloatRect &World::getBounds() const
{
    return m_bounds;
}

const engine::FloatVec2 &World::getPlayerStartingPosition() const
{
    return m_playerStartingPosition;
}

void World::forEachEntity(std::function <void(Entity &)> func) const
{
    if(!func)
        throw engine::Exception{"Function is nullptr."};

    for(auto it = m_entities_wantUpdate.begin(); it != m_entities_wantUpdate.end(); ++it) {
        E_DASSERT(it->second, "Entity is nullptr.");
        func(*it->second);
    }

    for(auto it = m_entities_dontWantUpdate.begin(); it != m_entities_dontWantUpdate.end(); ++it) {
        E_DASSERT(it->second, "Entity is nullptr.");
        func(*it->second);
    }
}

void World::playAmbientMusic(bool play)
{
    if(play)
        m_birdsAmbience.play();
    else
        m_birdsAmbience.stop();
}

std::shared_ptr <ElectricitySystem> World::addElectricitySystem(std::shared_ptr <Structure> structureMember)
{
    if(!structureMember)
        throw engine::Exception{"Tried to add electricity system with nullptr structure."};

    if(!structureMember->isInWorld())
        throw engine::Exception{"Tried to add structure which isn't in world to electricity system."};

    const auto &electricityComponent = structureMember->getElectricityComponent();

    if(electricityComponent.isInAnyElectricitySystem())
        electricityComponent.getElectricitySystem().removeStructure(*structureMember);

    m_electricitySystems.emplace_back(ElectricitySystem::create());
    m_electricitySystems.back()->addStructure(structureMember);

    return m_electricitySystems.back();
}

const std::vector <std::shared_ptr <ElectricitySystem>> &World::getElectricitySystems() const
{
    return m_electricitySystems;
}

const std::unordered_map <int, std::shared_ptr <Structure>> &World::getStructuresUsingElectricity() const
{
    return m_structuresUsingElectricity;
}

World::~World()
{
}

void World::useWorldPartFreePosFinderFieldAt(const engine::FloatVec2 &pos)
{
    auto *worldPart = getWorldPart(pos);

    if(worldPart) {
        const auto &tilePosition = worldPart->getTilePosition();

        worldPart->useFreePosFinderFieldAt(pos.moved(-tilePosition.x * WorldPart::k_terrainSize,
                                                     -tilePosition.y * WorldPart::k_terrainSize));
    }
}

void World::useWorldPartFreePosFinderFieldAt(const engine::FloatVec3 &pos)
{
    useWorldPartFreePosFinderFieldAt({pos.x, pos.z});
}

void World::updateElectricitySystems()
{
    // we need a copy because the container can be modified
    auto electricitySystemsCopy = m_electricitySystems;

    for(auto &elem : electricitySystemsCopy) {
        E_DASSERT(elem, "Electricity system is nullptr.");

        if(!elem->isEmpty())
            elem->update();
    }

    for(auto it = m_electricitySystems.begin(); it != m_electricitySystems.end();) {
        if((*it)->isEmpty())
            it = m_electricitySystems.erase(it);
        else
            ++it;
    }
}

void World::addToQuickAccessCachedEntities(const std::shared_ptr <Entity> &entity)
{
    if(!entity)
        return;

    auto structure = std::dynamic_pointer_cast <Structure> (entity);

    if(structure && structure->getDef().usesElectricity())
        m_structuresUsingElectricity.emplace(structure->getEntityID(), structure);
}

void World::removeFromQuickAccessCachedEntities(const Entity &entity)
{
    auto it1 = m_structuresUsingElectricity.find(entity.getEntityID());

    if(it1 != m_structuresUsingElectricity.end())
        m_structuresUsingElectricity.erase(it1);
}

const std::string World::k_birdsAmbiencePath = "music/birds.ogg";

} // namespace app
