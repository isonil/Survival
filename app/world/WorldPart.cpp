#include "WorldPart.hpp"

#include "engine/app3D/defs/TerrainDef.hpp"
#include "engine/app3D/sceneNodes/Terrain.hpp"
#include "engine/app3D/sceneNodes/Water.hpp"
#include "engine/app3D/managers/SceneManager.hpp"
#include "engine/app3D/managers/PhysicsManager.hpp"
#include "engine/app3D/physics/RigidBody.hpp"
#include "engine/app3D/physics/HeightMapShape.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/util/DefDatabase.hpp"
#include "engine/util/DHondtDistribution.hpp"
#include "engine/util/Random.hpp"
#include "../defs/DefsCache.hpp"
#include "../defs/WorldPartDef.hpp"
#include "../entities/Mineable.hpp"
#include "../entities/Item.hpp"
#include "../itemContainers/MultiSlotItemContainer.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "World.hpp"
#include "WorldPartFreePosFinder.hpp"
#include "WorldPartTopographyInfo.hpp"

namespace app
{

void WorldPart::expose(engine::DataFile::Node &node)
{
    TRACK;

    node.var(m_worldPartDef_defName, "worldPartDef");
    node.var(m_tilePosition, "tilePosition");

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        auto &defDatabase = Global::getCore().getDefDatabase();

        m_worldPartDef = defDatabase.getDef <WorldPartDef> (m_worldPartDef_defName);

        if(m_tilePosition.x < 2 || m_tilePosition.y < 2)
            throw engine::Exception{"World part tile position x and y must be at least 2."};

        addTerrainAndWater();
    }
}

void WorldPart::makeItWaterWorldPart(const engine::IntVec2 &tilePosition)
{
    auto &defsCache = Global::getCore().getDefsCache();

    m_worldPartDef_defName = defsCache.WorldPart_Water->getDefName();
    m_worldPartDef = defsCache.WorldPart_Water;
    m_tilePosition = tilePosition;

    if(m_tilePosition.x < 0 || m_tilePosition.y < 0)
        throw engine::Exception{"Water world part tile position can't be negative."};

    addTerrainAndWater();
}

void WorldPart::generateEntities(World &world)
{
    TRACK;

    if(!m_freePosFinder)
        throw engine::Exception{"World part free pos finder is nullptr."};

    E_DASSERT(m_terrain, "Terrain is nullptr.");
    E_DASSERT(m_worldPartDef, "World part def is nullptr.");

    const engine::FloatVec3 thisWorldPartPosOffset{m_tilePosition.x * k_terrainSize, 0.f, m_tilePosition.y * k_terrainSize};

    // first, spawn all prespawned entities

    for(const auto &prespawnedEntity : m_worldPartDef->getPrespawnedEntities()) {
        auto entity = Entity::createFromDef(world.getUniqueEntityID(), prespawnedEntity.getEntityDefPtr());

        auto pos = thisWorldPartPosOffset + prespawnedEntity.getPosition();

        if(prespawnedEntity.shouldAdjustPositionToTerrain())
            pos.y = getHeight({prespawnedEntity.getPosition().x,
                               prespawnedEntity.getPosition().z});

        entity->setFactionDef(prespawnedEntity.getFactionDefPtr());
        entity->setInWorldPosition(pos);
        entity->setInWorldRotation(prespawnedEntity.getRotation());

        world.addEntity(entity);

        if(entity->hasSearchableItemContainer()) {
            const auto &container = entity->getSearchableItemContainer();

            for(const auto &elem : prespawnedEntity.getParams().getSearchableItemContainerItems().getItems()) {
                const auto &item = std::make_shared <Item> (world.getUniqueEntityID(), elem.getItemDefPtr(), elem.getStack());
                container->tryAddItem(item);
            }
        }
    }

    // now spawn mineables

    int totalMineablesToGenerate{m_worldPartDef->getRandomMineablesCount()};
    const auto &randomMineables = m_worldPartDef->getRandomMineables();

    const auto &distributedCount = engine::DHondtDistribution::distribute(randomMineables.begin(), randomMineables.end(), [](const auto &m) {
        return m.getDensity();
    }, totalMineablesToGenerate);

    E_DASSERT(distributedCount.size() == randomMineables.size(), "Invalid distributed count size.");

    for(size_t i = 0; i < distributedCount.size(); ++i) {
        for(int j = 0; j < distributedCount[i]; ++j) {
            const auto &posOptional = m_freePosFinder->getRandomPosMatching(randomMineables[i].getPlacementPredicates());

            if(!posOptional) {
                E_WARNING("Could not generate full world part because could not find free pos to place Entity.");
                return;
            }

            const auto &pos = thisWorldPartPosOffset + *posOptional;

            const auto &mineableDef = randomMineables[i].getMineableDefPtr();
            const auto &mineable = std::make_shared <Mineable> (world.getUniqueEntityID(), mineableDef);

            engine::FloatVec3 rot{engine::Random::rangeInclusive(-5.f, 5.f), engine::Random::rangeExclusive(0.f, 360.f), 0.f};

            mineable->setInWorldPosition(pos);
            mineable->setInWorldRotation(rot);

            world.addEntity(mineable);
        }
    }
}

const engine::IntVec2 &WorldPart::getTilePosition() const
{
    return m_tilePosition;
}

float WorldPart::getHeight(const engine::FloatVec2 &pos) const
{
    E_DASSERT(m_terrain, "Terrain is nullptr."); // TODO: Exception

    return m_terrain->getHeight(pos);
}

float WorldPart::getSlope(const engine::FloatVec2 &pos) const
{
    E_DASSERT(m_topography, "Topography is nullptr."); // TODO: Exception

    return m_topography->getSlope(pos);
}

GroundType WorldPart::getGroundType(const engine::FloatVec2 &pos) const
{
    if(getHeight(pos) < k_waterHeight)
        return GroundType::UnderWater;

    if(getSlope(pos) > k_slopeGroundTypeSlopeThreshold)
        return GroundType::Slope; // usually rocks

    E_DASSERT(m_terrain, "Terrain is nullptr.");

    int groundType{m_terrain->getDef().getMostDominantGroundTextureIndex(pos)};

    // for example GroundType::Ground1 means that red color was the most dominant on the splat map

    switch(groundType) {
    case 0:
        return GroundType::Ground1; // usually sand (depends on WorldPart)
    case 1:
        return GroundType::Ground2; // usually grass
    case 2:
        return GroundType::Ground3; // usually grass or mud
    default:
        return GroundType::Ground1;
    }
}

std::experimental::optional <engine::FloatVec3> WorldPart::getRandomPosMatching_worldPos(const PlacementPredicates &predicates)
{
    if(!m_freePosFinder)
        throw engine::Exception{"Free pos finder is nullptr."};

    engine::FloatVec3 offset{m_tilePosition.x * k_terrainSize,
                             0.f,
                             m_tilePosition.y * k_terrainSize};

    const auto &posOptional = m_freePosFinder->getRandomPosMatching(predicates);

    if(!posOptional)
        return posOptional;

    return *posOptional + offset;
}

std::pair <engine::FloatVec3, bool> WorldPart::getPathFoundNextCheckpoint_worldPos(const engine::FloatVec2 &from, const engine::FloatVec2 &to)
{
    if(!m_freePosFinder)
        throw engine::Exception{"Free pos finder is nullptr."};

    engine::FloatVec2 offset{m_tilePosition.x * k_terrainSize,
                             m_tilePosition.y * k_terrainSize};

    engine::FloatVec3 offset3d{offset.x, 0.f, offset.y};

    engine::FloatVec3 checkpoint;
    bool clearWay{};

    std::tie(checkpoint, clearWay) = m_freePosFinder->getPathFoundNextCheckpoint(from - offset, to - offset);

    return std::make_pair(checkpoint + offset3d, clearWay);
}

void WorldPart::setFreePosFinderDirty()
{
    if(!m_freePosFinder)
        throw engine::Exception{"Free pos finder is nullptr."};

    m_freePosFinder->setDirty();
}

void WorldPart::useFreePosFinderFieldAt(const engine::FloatVec2 &pos)
{
    if(!m_freePosFinder)
        throw engine::Exception{"Free pos finder is nullptr."};

    m_freePosFinder->useFieldAt(pos);
}

WorldPartDef &WorldPart::getDef() const
{
    if(!m_worldPartDef)
        throw engine::Exception{"World part def is nullptr."};

    return *m_worldPartDef;
}

void WorldPart::addTerrainAndWater()
{
    TRACK;

    E_DASSERT(m_worldPartDef, "World part def is nullptr.");

    auto &core = Global::getCore();
    auto &device = core.getDevice();
    auto &sceneManager = device.getSceneManager();
    auto &physicsManager = device.getPhysicsManager();
    const auto &terrainDef = m_worldPartDef->getTerrainDefPtr();

    engine::FloatVec3 offset{m_tilePosition.x * k_terrainSize, 0.f, m_tilePosition.y * k_terrainSize};

    m_terrain = sceneManager.addTerrain(terrainDef);
    m_terrain->setPosition(offset);

    m_topography = std::make_shared <WorldPartTopographyInfo> (*m_terrain);
    m_freePosFinder = std::make_shared <WorldPartFreePosFinder> (*m_terrain, m_topography, offset);

    m_water = sceneManager.addWater(terrainDef);
    m_water->setPosition({m_tilePosition.x * k_terrainSize + k_terrainSize * 0.5f, k_waterHeight, m_tilePosition.y * k_terrainSize + k_terrainSize * 0.5f});
    m_water->setScale({k_terrainSize, 1.f, k_terrainSize});

    if(!m_terrain->isFlat()) {
        const auto &shape = std::make_shared <engine::app3D::HeightMapShape> (m_terrain->getHeightGrid());

        // we need to set proper local scaling, based on height grid size, and actual scale

        float localScaling{m_terrain->getDef().getScale() / shape->getMeshSideSize()};

        shape->setLocalScaling({localScaling, 1.f, localScaling});

        m_terrainRigidBody = physicsManager.addRigidBody(shape, 0.f, -1, {k_terrainSize * 0.5f, shape->getHeight() * 0.5f, k_terrainSize * 0.5f});
        m_terrainRigidBody->setPosition(offset);
        m_terrainRigidBody->setFriction(0.5f, 0.5f);
    }
    else
        m_terrainRigidBody.reset();
}

const float WorldPart::k_terrainSize{500.f};
const float WorldPart::k_waterHeight{15.f};
const float WorldPart::k_slopeGroundTypeSlopeThreshold{0.27f};

} // namespace app
