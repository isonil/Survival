#include "PointedEntity.hpp"

#include "engine/app3D/managers/SceneManager.hpp"
#include "engine/app3D/Device.hpp"
#include "../world/World.hpp"
#include "../entities/Entity.hpp"
#include "../entities/Character.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "ThisPlayer.hpp"

namespace app
{

void PointedEntity::update()
{
    TRACK;

    m_pointedEntity.reset();

    auto &core = Global::getCore();
    auto &thisPlayer = core.getThisPlayer();
    const auto &structureCurrentlyDesignated = thisPlayer.getStructureCurrentlyDesignated();

    if(structureCurrentlyDesignated.hasStructure())
        return;

    auto &device = core.getDevice();
    const auto &cameraLookVec = device.getSceneManager().getCameraLookVec();

    const auto &character = thisPlayer.getCharacter();

    const auto &rayFrom = character.getEyesPosition();
    const auto &rayTo = rayFrom + cameraLookVec * k_rayLengthToPickEntity;

    engine::FloatVec3 hitPos;
    int hitBodyUserIndex{-1};

    bool ray{character.rayTest_notMe(rayFrom, rayTo, engine::app3D::CollisionFilter::AllReal, hitPos, hitBodyUserIndex)};

    auto &world = core.getWorld();

    // only indices >= 0 are associated with entities
    if(ray && hitBodyUserIndex >= 0 && world.entityExists(hitBodyUserIndex)) {
        const auto &entity = core.getWorld().getEntityPtr(hitBodyUserIndex);

        E_DASSERT(entity, "Entity is nullptr.");

        core.getWorld().getEntity(hitBodyUserIndex).onPointedByPlayer();
        m_pointedEntity = entity;
    }
}

bool PointedEntity::isAny() const
{
    return !m_pointedEntity.expired();
}

Entity &PointedEntity::getEntity() const
{
    const auto &shared = m_pointedEntity.lock();

    if(!shared)
        throw engine::Exception{"Tried to get currently pointed entity, but it is nullptr. This should have been checked before."};

    return *shared;
}

std::shared_ptr <Entity> PointedEntity::getEntityPtr() const
{
    const auto &shared = m_pointedEntity.lock();

    if(!shared)
        throw engine::Exception{"Tried to get currently pointed entity, but it is nullptr. This should have been checked before."};

    return shared;
}

const float PointedEntity::k_rayLengthToPickEntity{5.f};

} // namespace app
