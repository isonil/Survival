#ifndef APP_MINEABLE_HPP
#define APP_MINEABLE_HPP

#include "engine/util/Trace.hpp"
#include "Entity.hpp"

#include <memory>

namespace engine { namespace app3D { class RigidBody; class Model; } }

namespace app
{

class MineableDef;
class ItemDef;

class ResourceInMineable
{
public:
    ResourceInMineable(const std::shared_ptr <ItemDef> &itemDef, int count);

    void subtract(int count);

    const std::shared_ptr <ItemDef> &getItemDefPtr() const;
    ItemDef &getItemDef() const;
    int getCount() const;

private:
    int m_count;
    std::shared_ptr <ItemDef> m_itemDef;
};

class Mineable : public Entity, public engine::Tracked <Mineable>
{
public:
    Mineable(int entityID, const std::shared_ptr <MineableDef> &def);

    bool wantsEverInWorldUpdate() const override;
    void setInWorldPosition(const engine::FloatVec3 &pos) override;
    void setInWorldRotation(const engine::FloatVec3 &rot) override;
    bool wantsToBeRemovedFromWorld() const override;
    bool blocksWorldPartFreePosFinderField() const override;
    std::shared_ptr <EffectDef> getOnHitEffectDefPtr() const override;
    std::string getName() const override;
    void onInWorldUpdate() override;
    void onSpawnedInWorld() override;
    void onRemovedFromWorld() override;
    void onPointedByPlayer() override;
    void onDraw2DInfoWhenPointed() override;
    void onItemUsedOnMe(Entity &doer, const Item &item) override;

    bool hasAnyResources() const;
    const std::vector <ResourceInMineable> &getResources() const;

private:
    using base = Entity;

    void initResources();

    std::shared_ptr <MineableDef> m_def;
    std::shared_ptr <engine::app3D::Model> m_model;
    std::shared_ptr <engine::app3D::RigidBody> m_rigidBody;
    std::vector <ResourceInMineable> m_resources;
    int m_durability;
};

} // namespace app

#endif // APP_MINEABLE_HPP
