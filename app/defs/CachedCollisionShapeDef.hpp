#ifndef APP_CACHED_COLLISION_SHAPE_DEF_HPP
#define APP_CACHED_COLLISION_SHAPE_DEF_HPP

#include "engine/util/Trace.hpp"
#include "engine/util/Def.hpp"
#include "engine/util/Enum.hpp"
#include "engine/util/Vec3.hpp"

#include <memory>

namespace engine { namespace app3D { class CollisionShape; } }

namespace app
{

class Entity;

class CachedCollisionShapeDef : public engine::Def, public engine::Tracked <CachedCollisionShapeDef>
{
public:
    ENUM_DECL(Type,
        Box,
        Capsule,
        Cone,
        Cylinder,
        Sphere,
        Plane,
        ConvexHull,
        BvhTriangleMesh
    );

    CachedCollisionShapeDef();

    void expose(engine::DataFile::Node &node) override;

    void addPoofEffect(const engine::FloatVec3 &pos, const engine::FloatVec3 &rot) const;

    const engine::app3D::CollisionShape &getCollisionShape() const;
    const std::shared_ptr <engine::app3D::CollisionShape> &getCollisionShapePtr() const;
    const engine::FloatVec3 &getPosOffset() const;
    float getHeight() const;
    const CachedCollisionShapeDef::Type &getType() const;
    const engine::FloatVec3 &getBoxSize() const;

private:
    using base = Def;

    float m_radius;
    float m_height;
    float m_planeConstant;

    Type m_type;
    std::shared_ptr <engine::app3D::CollisionShape> m_collisionShape;
    engine::FloatVec3 m_size;
    engine::FloatVec3 m_planeNormal;
    std::string m_meshPath;
    engine::FloatVec3 m_posOffset;
};

} // namespace app

#endif // APP_CACHED_COLLISION_SHAPE_DEF_HPP
