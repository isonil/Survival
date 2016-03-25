#include "StructureCurrentlyDesignated.hpp"

#include "../entities/components/ElectricityComponent.hpp"
#include "../entities/components/PlayerComponent.hpp"
#include "engine/app3D/managers/SceneManager.hpp"
#include "engine/app3D/managers/PhysicsManager.hpp"
#include "engine/app3D/sceneNodes/Model.hpp"
#include "engine/app3D/physics/CollisionDetector.hpp"
#include "../defs/StructureRecipeDef.hpp"
#include "../defs/StructureDef.hpp"
#include "../defs/CachedCollisionShapeDef.hpp"
#include "../world/World.hpp"
#include "../world/ElectricitySystem.hpp"
#include "../entities/Structure.hpp"
#include "../entities/Character.hpp"
#include "engine/app3D/Device.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "ThisPlayer.hpp"

namespace app
{

void StructureCurrentlyDesignated::update()
{
    updateModelAndCollisionDetector();

    if(hasStructure() && getStructureRecipeDef().getStructureDef().usesElectricity()) {
        const auto &electricitySystems = Global::getCore().getWorld().getElectricitySystems();

        for(const auto &elem : electricitySystems) {
            E_DASSERT(elem, "Electricity system is nullptr.");
            elem->setOverlay(ElectricitySystem::Overlay::GraySymbols);
        }
    }

    const auto &electricityConnection = getElectricityConnection();

    if(electricityConnection)
        electricityConnection->getElectricityComponent().getElectricitySystem().setOverlay(ElectricitySystem::Overlay::NormalSymbolsAndLines);
}

void StructureCurrentlyDesignated::draw3D()
{
    if(!hasStructure())
        return;

    const auto &electricityConnection = getElectricityConnection();

    if(!electricityConnection)
        return;

    auto &sceneManager = Global::getCore().getDevice().getSceneManager();

    sceneManager.setLineMaterial();
    sceneManager.draw3DLine(getDesignatedPlacement().pos, electricityConnection->getCenterPosition(), {0.5f, 0.5f, 0.5f});
}

bool StructureCurrentlyDesignated::hasStructure() const
{
    return static_cast <bool> (m_recipe);
}

void StructureCurrentlyDesignated::setStructureRecipeDef(const std::shared_ptr <StructureRecipeDef> &recipe)
{
    if(!recipe)
        throw engine::Exception{"Tried to set nullptr structure recipe def as current one."};

    m_recipe = recipe;

    addModelAndCollisionDetector();
}

void StructureCurrentlyDesignated::removeStructure()
{
    m_recipe.reset();
    m_model.reset();
    m_collisionDetector.reset();
}

bool StructureCurrentlyDesignated::tryBuild()
{
    TRACK;

    if(!hasStructure())
        return false;

    E_DASSERT(m_recipe, "Structure recipe def is nullptr.");

    const auto &placement = getDesignatedPlacement();

    if(placement.canPlace) {
        auto &core = Global::getCore();
        auto &playerCharacter = core.getThisPlayer().getCharacter();
        auto &playerInventory = playerCharacter.getInventory();

        if(!playerInventory.canAfford(m_recipe->getPrice()))
            return false;

        playerInventory.pay(m_recipe->getPrice());

        auto &world = core.getWorld();

        const auto &structure = std::make_shared <Structure> (world.getUniqueEntityID(), m_recipe->getStructureDefPtr());

        structure->setFactionDef(playerCharacter.getFactionDefPtr());
        structure->setOwner(playerCharacter);
        structure->setInWorldPosition(placement.pos);
        structure->setInWorldRotation(placement.rot);

        const auto &structureDef = m_recipe->getStructureDef();

        structureDef.getCachedCollisionShapeDef().addPoofEffect(placement.pos, placement.rot);

        if(structureDef.usesElectricity()) {
            auto electricityConnection = getElectricityConnection();

            world.addEntity(structure);

            if(electricityConnection && electricityConnection->getElectricityComponent().isInAnyElectricitySystem())
                // add to existing electricity system
                electricityConnection->getElectricityComponent().getElectricitySystem().addStructure(structure, electricityConnection);
            else // create new electricity system
                world.addElectricitySystem(structure);
        }
        else
            world.addEntity(structure);

        playerCharacter.getPlayerComponent().getSkills().addConstructingExp(m_recipe->getConstructingExpForConstructing());

        return true;
    }

    return false;
}

StructureRecipeDef &StructureCurrentlyDesignated::getStructureRecipeDef() const
{
    if(!m_recipe)
        throw engine::Exception{"Structure recipe def is nullptr. This should have been checked before."};

    return *m_recipe;
}

const std::shared_ptr <StructureRecipeDef> &StructureCurrentlyDesignated::getStructureRecipeDefPtr() const
{
    if(!m_recipe)
        throw engine::Exception{"Structure recipe def is nullptr. This should have been checked before."};

    return m_recipe;
}

StructureCurrentlyDesignated::Placement::Placement()
    : canPlace{}
{
}

StructureCurrentlyDesignated::Placement::Placement(bool canPlace, const engine::FloatVec3 &pos, const engine::FloatVec3 &rot)
    : canPlace{canPlace},
      pos{pos},
      rot{rot}
{
}

void StructureCurrentlyDesignated::updateModelAndCollisionDetector()
{
    TRACK;

    if(!hasStructure()) {
        m_model.reset();
        m_collisionDetector.reset();
    }
    else {
        E_DASSERT(m_model, "Model is nullptr.");

        const auto &placement = getDesignatedPlacement();

        m_model->setPosition(placement.pos);
        m_model->setRotation(placement.rot);

        if(placement.canPlace)
            m_model->highlightNextFrame(k_canPlaceColor);
        else
            m_model->highlightNextFrame(k_cantPlaceColor);
    }
}

StructureCurrentlyDesignated::Placement StructureCurrentlyDesignated::getDesignatedPlacement()
{
    TRACK;

    if(!hasStructure())
        return {};

    auto &core = Global::getCore();
    auto &device = core.getDevice();
    const auto &sceneManager = device.getSceneManager();
    const auto &playerCharacter = core.getThisPlayer().getCharacter();
    const auto &eyesPosition = playerCharacter.getEyesPosition();
    const auto &rayEnd = eyesPosition + sceneManager.getCameraLookVec() * k_distanceFromCameraToDesignatedPos;

    engine::FloatVec3 rot{0.f, sceneManager.getCameraRotation().y, 0.f};
    engine::FloatVec3 rayHitPos;
    int hitEntityID{-1};

    bool didHit{playerCharacter.rayTest_notMe(eyesPosition, rayEnd, engine::app3D::CollisionFilter::AllReal, rayHitPos, hitEntityID)};

    if(didHit)
        return getDesignatedPlacement_rayHit(rayHitPos, hitEntityID, rot);
    else
        return getDesignatedPlacement_rayNotHit(rayEnd, rot);
}

StructureCurrentlyDesignated::Placement StructureCurrentlyDesignated::getDesignatedPlacement_rayHit(const engine::FloatVec3 &rayHitPos, int hitEntityID, const engine::FloatVec3 &defaultRot)
{
    TRACK;

    auto &world = Global::getCore().getWorld();

    E_DASSERT(m_collisionDetector, "Collision detector is nullptr.");
    m_collisionDetector->setPosition(rayHitPos);
    m_collisionDetector->setRotation(defaultRot);

    std::vector <std::pair <engine::FloatVec3, engine::FloatVec3>> possibilities;

    if(hitEntityID >= 0) {
        const auto &hitEntity = world.getEntity(hitEntityID);

        if(hitEntity.canBuildOnTopOfIt()) {
            E_DASSERT(m_recipe, "Structure recipe def is nullptr.");

            const auto &snapped = hitEntity.trySnapToMe(m_recipe->getStructureDef(), rayHitPos);

            if(snapped.empty())
                possibilities.emplace_back(rayHitPos, defaultRot);
            else
                possibilities.insert(possibilities.end(), snapped.begin(), snapped.end());
        }
    }
    else {
        // we hit a non-entity, so we assume that we can build on top of it

        possibilities.emplace_back(rayHitPos, defaultRot);
    }

    if(possibilities.empty())
        return {false, rayHitPos, defaultRot};

    // TODO: check all possibilities, not just one (there was a problem with collision detector not updating midframe)

    m_collisionDetector->setPosition(possibilities[0].first);
    m_collisionDetector->setRotation(possibilities[0].second);

    const auto &collided = m_collisionDetector->getCollidedObjectsUserIndices();

    bool forbidden{};

    for(const auto &index : collided) {
        if(index < 0)
            continue;

        const auto &entity = world.getEntity(index);

        if(!entity.canBuildOnTopOfIt()) {
            forbidden = true;
            break;
        }

        if(entity.getInWorldPosition().getDistanceSq(possibilities[0].first) < k_minDistBetweenEntities * k_minDistBetweenEntities) {
            forbidden = true;
            break;
        }
    }

    return {!forbidden, possibilities[0].first, possibilities[0].second};
}

StructureCurrentlyDesignated::Placement StructureCurrentlyDesignated::getDesignatedPlacement_rayNotHit(const engine::FloatVec3 &designatedPos, const engine::FloatVec3 &defaultRot)
{
    TRACK;

    auto &world = Global::getCore().getWorld();

    E_DASSERT(m_collisionDetector, "Collision detector is nullptr.");
    m_collisionDetector->setPosition(designatedPos);
    m_collisionDetector->setRotation(defaultRot);

    const auto &collided = m_collisionDetector->getCollidedObjectsUserIndices();

    bool anyCanPlace{};
    bool anyCantPlace{};

    for(const auto &index : collided) {
        if(index < 0) {
            // if collided with non-entity, then we assume that player can build on top of it

            anyCanPlace = true;
            continue;
        }

        const auto &entity = world.getEntity(index);

        if(entity.canBuildOnTopOfIt())
            anyCanPlace = true;
        else {
            anyCantPlace = true;
            break;
        }

        if(entity.getInWorldPosition().getDistanceSq(designatedPos) < k_minDistBetweenEntities * k_minDistBetweenEntities) {
            anyCantPlace = true;
            break;
        }
    }

    bool canPlace{anyCanPlace && !anyCantPlace};

    return {canPlace, designatedPos, defaultRot};
}

std::shared_ptr <Structure> StructureCurrentlyDesignated::getElectricityConnection()
{
    if(!hasStructure())
        return {};

    if(!getStructureRecipeDef().getStructureDef().usesElectricity())
        return {};

    auto &core = Global::getCore();
    const auto &structuresUsingElectricity = core.getWorld().getStructuresUsingElectricity();
    const auto &playerFactionDef = core.getThisPlayer().getCharacter().getFactionDef();
    const auto &designatedPos = getDesignatedPlacement().pos;

    std::shared_ptr <Structure> found;

    for(const auto &mapElem : structuresUsingElectricity) {
        const auto &elem = mapElem.second;

        E_DASSERT(elem, "Structure is nullptr.");

        if(!elem->getElectricityComponent().isInAnyElectricitySystem())
            continue;

        float distSq{designatedPos.getDistanceSq(elem->getInWorldPosition())};

        if(distSq > ElectricitySystem::k_maxConnectionLength * ElectricitySystem::k_maxConnectionLength)
            continue;

        if(&elem->getFactionDef() != &playerFactionDef)
            continue;

        if(!found || distSq < found->getInWorldPosition().getDistanceSq(designatedPos))
            found = elem;
    }

    return found;
}

void StructureCurrentlyDesignated::addModelAndCollisionDetector()
{
    TRACK;

    if(!hasStructure())
        return;

    E_DASSERT(m_recipe, "Structure recipe def is nullptr.");

    auto &device = Global::getCore().getDevice();
    auto &sceneManager = device.getSceneManager();
    auto &physicsManager = device.getPhysicsManager();
    const auto &structureDef = m_recipe->getStructureDef();
    const auto &modelDefPtr = structureDef.getModelDefPtr();
    const auto &cachedCollisionShape = structureDef.getCachedCollisionShapeDef();

    m_model = sceneManager.addModel(modelDefPtr);

    m_collisionDetector = physicsManager.addCollisionDetector(cachedCollisionShape.getCollisionShapePtr(), engine::app3D::CollisionFilter::GhostMisc, engine::app3D::CollisionFilter::AllReal, cachedCollisionShape.getPosOffset());

    updateModelAndCollisionDetector();
}

const float StructureCurrentlyDesignated::k_distanceFromCameraToDesignatedPos{4.f};
const float StructureCurrentlyDesignated::k_minDistBetweenEntities{0.2f};
const engine::Color StructureCurrentlyDesignated::k_canPlaceColor{0.3f, 1.f, 0.3f};
const engine::Color StructureCurrentlyDesignated::k_cantPlaceColor{1.f, 0.3f, 0.3f};

} // namespace app
