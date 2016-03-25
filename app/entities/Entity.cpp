#include "Entity.hpp"

#include "../defs/DefsCache.hpp"
#include "../defs/FactionDef.hpp"
#include "../defs/EntityDef.hpp"
#include "../defs/CharacterDef.hpp"
#include "../defs/ItemDef.hpp"
#include "../defs/MineableDef.hpp"
#include "../defs/StructureDef.hpp"
#include "../defs/EffectDef.hpp"
#include "../world/World.hpp"
#include "../world/WorldPart.hpp"
#include "engine/app3D/IrrlichtConversions.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "Character.hpp"
#include "Item.hpp"
#include "Mineable.hpp"
#include "Structure.hpp"

namespace app
{

std::shared_ptr <Entity> Entity::createFromDef(int entityID, const std::shared_ptr <EntityDef> &entityDef)
{
    auto mineableDef = std::dynamic_pointer_cast <MineableDef> (entityDef);

    if(mineableDef)
        return std::make_shared <Mineable> (entityID, mineableDef);

    auto structureDef = std::dynamic_pointer_cast <StructureDef> (entityDef);

    if(structureDef)
        return std::make_shared <Structure> (entityID, structureDef);

    auto characterDef = std::dynamic_pointer_cast <CharacterDef> (entityDef);

    if(characterDef)
        return std::make_shared <Character> (entityID, characterDef, false);

    auto itemDef = std::dynamic_pointer_cast <ItemDef> (entityDef);

    if(itemDef)
        return std::make_shared <Item> (entityID, itemDef);

    throw engine::Exception{"Entity def not handled."};
}

Entity::Entity(int entityID)
    : m_entityID{entityID},
      m_isInWorld{}
{
    if(m_entityID < 0) {
        throw engine::Exception{"Tried to construct Entity using negative entity ID. "
                                "Only >= 0 values represent valid entity IDs."};
    }

    m_factionDef = Global::getCore().getDefsCache().Faction_Neutral;
}

void Entity::onInWorldUpdate()
{
}

void Entity::onSpawnedInWorld()
{
    m_isInWorld = true;
}

void Entity::onRemovedFromWorld()
{
    m_isInWorld = false;
}

void Entity::setInWorldPosition(const engine::FloatVec3 &pos)
{
    m_pos = pos;
}

void Entity::setInWorldRotation(const engine::FloatVec3 &rot)
{
    m_rot = rot;
}

void Entity::onPointedByPlayer()
{
}

void Entity::onDraw2DInfoWhenPointed()
{
}

bool Entity::canBuildOnTopOfIt() const
{
    return false;
}

bool Entity::wantsToBeRemovedFromWorld() const
{
    return false;
}

void Entity::onItemUsedOnMe(Entity &doer, const Item &item)
{
}

std::vector <std::pair <engine::FloatVec3, engine::FloatVec3>> Entity::trySnapToMe(const StructureDef &structureDef, const engine::FloatVec3 &designatedPos) const
{
    return {};
}

bool Entity::canBeDeconstructed(const Character &doer) const
{
    return false;
}

bool Entity::canBeRevived(const Character &doer) const
{
    return false;
}

bool Entity::isKilled() const
{
    return false;
}

engine::FloatVec3 Entity::getAIAimPosition() const
{
    return getInWorldPosition();
}

int Entity::getAIPotentialTargetPriority() const
{
    return -1;
}

bool Entity::blocksWorldPartFreePosFinderField() const
{
    return false;
}

bool Entity::isPotentiallyMeleeReachableBy(const Character &character) const
{
    if(!isInWorld())
        return false;

    const auto &inWorldPosition = getInWorldPosition();

    if(inWorldPosition.y <= WorldPart::k_waterHeight)
        return true;

    auto &world = Global::getCore().getWorld();

    if(inWorldPosition.y < world.getHeight(inWorldPosition) + character.getDistanceBetweenFeetAndEyes() + ItemDef::k_meleeRange)
        return true;

    return false;
}

bool Entity::tryPickUpItem(std::shared_ptr <Item> item)
{
    return false;
}

bool Entity::hasSearchableItemContainer() const
{
    return false;
}

std::shared_ptr <MultiSlotItemContainer> Entity::getSearchableItemContainer() const
{
    throw engine::Exception{"This entity does not have any searchable item container. This should have been checked before."};
}

std::string Entity::getName() const
{
    return "Unnamed entity";
}

std::shared_ptr <EffectDef> Entity::getOnHitEffectDefPtr() const
{
    return Global::getCore().getDefsCache().Effect_OnHitGeneric;
}

void Entity::onDeconstructed(Character &doer)
{
}

void Entity::onRevived(Character &doer)
{
}

void Entity::onCharacterStepOnIt(Character &character)
{
}

void Entity::onHitGround(float force)
{
}

void Entity::onStoppedBusyAnimation()
{
}

int Entity::getEntityID() const
{
    return m_entityID;
}

const engine::FloatVec3 &Entity::getInWorldPosition() const
{
    return m_pos;
}

const engine::FloatVec3 &Entity::getInWorldRotation() const
{
    return m_rot;
}

bool Entity::isInWorld() const
{
    return m_isInWorld;
}

FactionDef &Entity::getFactionDef() const
{
    E_DASSERT(m_factionDef, "Faction def is nullptr.");
    return *m_factionDef;
}

const std::shared_ptr <FactionDef> &Entity::getFactionDefPtr() const
{
    E_DASSERT(m_factionDef, "Faction def is nullptr.");
    return m_factionDef;
}

void Entity::setFactionDef(const std::shared_ptr <FactionDef> &factionDef)
{
    if(!factionDef)
        throw engine::Exception{"Faction def is nullptr."};

    m_factionDef = factionDef;
}

void Entity::setInWorldRotationToFace(const engine::FloatVec2 &vec)
{
    const auto &pos = getInWorldPosition();
    engine::FloatVec3 vec3D{vec.x, pos.y, vec.y}; // 2d vs 3d

    const auto &irrVec = engine::app3D::IrrlichtConversions::toVector((vec3D - pos).normalized());
    const auto &rot = irrVec.getHorizontalAngle();

    setInWorldRotation({rot.X, rot.Y, rot.Z});
}

void Entity::setInWorldRotationToFace(const engine::FloatVec3 &vec)
{
    const auto &pos = getInWorldPosition();

    const auto &irrVec = engine::app3D::IrrlichtConversions::toVector((vec - pos).normalized());
    const auto &rot = irrVec.getHorizontalAngle();

    setInWorldRotation({rot.X, rot.Y, rot.Z});
}

engine::FloatVec3 Entity::rotateAsMe(const engine::FloatVec3 &vec) const
{
    const auto &rot = getInWorldRotation();
    auto irrVec = engine::app3D::IrrlichtConversions::toVector(vec);

    irr::core::matrix4 mat;
    mat.setRotationDegrees({rot.x, rot.y, rot.z});
    mat.rotateVect(irrVec);

    return {irrVec.X, irrVec.Y, irrVec.Z};
}

} // namespace app
