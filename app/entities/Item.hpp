#ifndef APP_ITEM_HPP
#define APP_ITEM_HPP

#include "engine/util/Trace.hpp"
#include "Entity.hpp"

#include <memory>

namespace engine { namespace app3D { class RigidBody; class Model; class CollisionDetector; } }

namespace app
{

class ItemDef;

class Item : public Entity, public engine::Tracked <Item>
{
public:
    Item(int entityID, const std::shared_ptr <ItemDef> &def, int stack = 1);

    bool wantsEverInWorldUpdate() const override;
    void setInWorldPosition(const engine::FloatVec3 &pos) override;
    void setInWorldRotation(const engine::FloatVec3 &rot) override;
    bool wantsToBeRemovedFromWorld() const override;
    std::string getName() const override;
    void onInWorldUpdate() override;
    void onSpawnedInWorld() override;
    void onRemovedFromWorld() override;
    void onPointedByPlayer() override;
    void onDraw2DInfoWhenPointed() override;

    void onUsedOnSomething(const engine::FloatVec3 &sourcePos, const engine::FloatVec3 &hitPos, int hitEntityID, Entity &doer) const;

    const ItemDef &getDef() const;
    const std::shared_ptr <ItemDef> &getDefPtr() const;
    int getStack() const;

    bool hasFullStack() const;
    std::shared_ptr <Item> split(int getThisMany);
    int tryMergeStack(Item &mergeWithThis, int tryTakeThisMany);

private:
    using base = Entity;

    int m_stack;
    std::shared_ptr <ItemDef> m_def;
    std::shared_ptr <engine::app3D::Model> m_model;
    std::shared_ptr <engine::app3D::RigidBody> m_rigidBody;
    std::shared_ptr <engine::app3D::CollisionDetector> m_collisionDetector;
    bool m_isPickedUp;
};

} // namespace app

#endif // APP_ITEM_HPP
