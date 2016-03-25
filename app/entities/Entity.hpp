#ifndef APP_ENTITY_HPP
#define APP_ENTITY_HPP

#include "engine/util/Trace.hpp"
#include "engine/util/Vec2.hpp"
#include "engine/util/Vec3.hpp"

namespace app
{

class StructureDef;
class Item;
class Character;
class FactionDef;
class EffectDef;
class MultiSlotItemContainer;
class EntityDef;

class Entity : public engine::Tracked <Entity>
{
public:
    static std::shared_ptr <Entity> createFromDef(int entityID, const std::shared_ptr <EntityDef> &entityDef);

    explicit Entity(int entityID);

    virtual bool wantsEverInWorldUpdate() const = 0;
    virtual void setInWorldPosition(const engine::FloatVec3 &pos);
    virtual void setInWorldRotation(const engine::FloatVec3 &rot);
    virtual bool canBuildOnTopOfIt() const;
    virtual bool wantsToBeRemovedFromWorld() const;
    virtual std::vector <std::pair <engine::FloatVec3, engine::FloatVec3>> trySnapToMe(const StructureDef &structureDef, const engine::FloatVec3 &designatedPos) const;
    virtual bool canBeDeconstructed(const Character &doer) const;
    virtual bool canBeRevived(const Character &doer) const;
    virtual bool isKilled() const;
    virtual engine::FloatVec3 getAIAimPosition() const;
    virtual int getAIPotentialTargetPriority() const;
    virtual bool blocksWorldPartFreePosFinderField() const;
    virtual bool isPotentiallyMeleeReachableBy(const Character &character) const;
    virtual bool tryPickUpItem(std::shared_ptr <Item> item);
    virtual bool hasSearchableItemContainer() const;
    virtual std::shared_ptr <MultiSlotItemContainer> getSearchableItemContainer() const;
    virtual std::string getName() const;
    virtual std::shared_ptr <EffectDef> getOnHitEffectDefPtr() const;
    virtual void onInWorldUpdate();
    virtual void onSpawnedInWorld();
    virtual void onRemovedFromWorld();
    virtual void onPointedByPlayer();
    virtual void onDraw2DInfoWhenPointed();
    virtual void onItemUsedOnMe(Entity &doer, const Item &item);
    virtual void onDeconstructed(Character &doer);
    virtual void onRevived(Character &doer);
    virtual void onCharacterStepOnIt(Character &character);
    virtual void onHitGround(float force);
    virtual void onStoppedBusyAnimation();

    virtual ~Entity() = default;

    int getEntityID() const;
    const engine::FloatVec3 &getInWorldPosition() const;
    const engine::FloatVec3 &getInWorldRotation() const;
    bool isInWorld() const;
    FactionDef &getFactionDef() const;
    const std::shared_ptr <FactionDef> &getFactionDefPtr() const;

    void setFactionDef(const std::shared_ptr <FactionDef> &factionDef);
    void setInWorldRotationToFace(const engine::FloatVec2 &vec);
    void setInWorldRotationToFace(const engine::FloatVec3 &vec);
    engine::FloatVec3 rotateAsMe(const engine::FloatVec3 &vec) const;

private:
    int m_entityID;
    bool m_isInWorld;
    engine::FloatVec3 m_pos;
    engine::FloatVec3 m_rot;

    std::shared_ptr <FactionDef> m_factionDef;
};

} // namespace app

#endif // APP_ENTITY_HPP
