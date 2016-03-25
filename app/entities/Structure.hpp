#ifndef APP_STRUCTURE_HPP
#define APP_STRUCTURE_HPP

#include "engine/app3D/physics/CollisionFilter.hpp"
#include "engine/util/Trace.hpp"
#include "Entity.hpp"

#include <memory>

namespace engine { namespace app3D { class RigidBody; class Model; } }

namespace app
{

class StructureDef;
class TurretComponent;
class ElectricityComponent;
class MultiSlotItemContainer;
class Effect;

class Structure : public Entity, public engine::Tracked <Structure>
{
public:
    Structure(int entityID, const std::shared_ptr <StructureDef> &def);

    bool wantsEverInWorldUpdate() const override;
    void setInWorldPosition(const engine::FloatVec3 &pos) override;
    void setInWorldRotation(const engine::FloatVec3 &rot) override;
    bool canBuildOnTopOfIt() const override;
    bool wantsToBeRemovedFromWorld() const override;
    std::vector <std::pair <engine::FloatVec3, engine::FloatVec3>> trySnapToMe(const StructureDef &structureDef, const engine::FloatVec3 &designatedPos) const override;
    bool canBeDeconstructed(const Character &doer) const override;
    bool isKilled() const override;
    engine::FloatVec3 getAIAimPosition() const override;
    int getAIPotentialTargetPriority() const override;
    std::shared_ptr <EffectDef> getOnHitEffectDefPtr() const override;
    bool hasSearchableItemContainer() const override;
    std::shared_ptr <MultiSlotItemContainer> getSearchableItemContainer() const override;
    std::string getName() const override;
    void onInWorldUpdate() override;
    void onSpawnedInWorld() override;
    void onRemovedFromWorld() override;
    void onPointedByPlayer() override;
    void onDraw2DInfoWhenPointed() override;
    void onItemUsedOnMe(Entity &doer, const Item &item) override;
    void onDeconstructed(Character &doer) override;
    void onCharacterStepOnIt(Character &character) override;

    engine::FloatVec3 getCenterPosition() const;

    const engine::FloatVec3 &getTurretHeadRotation() const;
    void setTurretHeadRotation(const engine::FloatVec3 &turretHeadRot);

    ElectricityComponent &getElectricityComponent() const;

    StructureDef &getDef() const;

    void setOwner(const Character &character);
    bool hasOwner() const;
    Character &getOwner() const;

    bool rayTest_notTurretHead(const engine::FloatVec3 &start, const engine::FloatVec3 &end, engine::app3D::CollisionFilter withWhatCollide, engine::FloatVec3 &outPos, int &outHitBodyUserIndex) const;

    ~Structure() override;

private:
    using base = Entity;

    void turret_setInWorldPosition(const engine::FloatVec3 &pos);
    void turret_setInWorldRotation(const engine::FloatVec3 &rot);
    void turret_onInWorldUpdate(const engine::FloatVec3 &rigidBodyPos, const engine::FloatVec3 &rigidBodyRot);
    void turret_onSpawnedInWorld(const engine::FloatVec3 &inWorldPos, const engine::FloatVec3 &inWorldRot);
    void turret_onRemovedFromWorld();

    static const engine::IntVec2 k_searchableItemContainerSize;

    std::shared_ptr <StructureDef> m_def;

    int m_ownerEntityID;

    std::shared_ptr <engine::app3D::Model> m_model;
    std::shared_ptr <engine::app3D::RigidBody> m_rigidBody;

    std::shared_ptr <engine::app3D::Model> m_turretHeadModel;
    std::shared_ptr <engine::app3D::RigidBody> m_turretHeadRigidBody;

    std::unique_ptr <Effect> m_effect;

    std::unique_ptr <TurretComponent> m_turretComponent;
    engine::FloatVec3 m_turretHeadRot;

    std::shared_ptr <ElectricityComponent> m_electricityComponent;

    std::shared_ptr <MultiSlotItemContainer> m_searchableItemContainer;

    int m_HP;
    bool m_shouldExplodeWhenRemovedFromWorld;
};

} // namespace app

#endif // APP_STRUCTURE_HPP
