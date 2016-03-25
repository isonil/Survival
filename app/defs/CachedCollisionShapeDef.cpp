#include "CachedCollisionShapeDef.hpp"

#include "engine/app3D/physics/CollisionShape.hpp"
#include "engine/app3D/physics/BoxShape.hpp"
#include "engine/app3D/physics/CapsuleShape.hpp"
#include "engine/app3D/physics/ConeShape.hpp"
#include "engine/app3D/physics/CylinderShape.hpp"
#include "engine/app3D/physics/SphereShape.hpp"
#include "engine/app3D/physics/StaticPlaneShape.hpp"
#include "engine/app3D/physics/ConvexHullShape.hpp"
#include "engine/app3D/physics/BvhTriangleMeshShape.hpp"
#include "engine/app3D/managers/ResourcesManager.hpp"
#include "../defs/DefsCache.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/app3D/IrrlichtConversions.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "../EffectsPool.hpp"

namespace app
{

ENUM_DEF(CachedCollisionShapeDef::Type, Type,
    Box,
    Capsule,
    Cone,
    Cylinder,
    Sphere,
    Plane,
    ConvexHull,
    BvhTriangleMesh
);

CachedCollisionShapeDef::CachedCollisionShapeDef()
    : m_radius{},
      m_height{},
      m_planeConstant{}
{
}

void CachedCollisionShapeDef::expose(engine::DataFile::Node &node)
{
    TRACK;

    base::expose(node);

    node.var(m_type, "type");
    node.var(m_height, "height", 0.f);
    node.var(m_radius, "radius", 0.f);
    node.var(m_size, "size", {});
    node.var(m_planeNormal, "planeNormal", {0.f, 1.f, 0.f});
    node.var(m_planeConstant, "planeConstant", 0.f);
    node.var(m_meshPath, "meshPath", "");
    node.var(m_posOffset, "posOffset", {});

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        if(m_type == Type::Box)
            m_collisionShape = std::make_shared <engine::app3D::BoxShape> (m_size);
        else if(m_type == Type::Capsule)
            m_collisionShape = std::make_shared <engine::app3D::CapsuleShape> (m_radius, m_height);
        else if(m_type == Type::Cone)
            m_collisionShape = std::make_shared <engine::app3D::ConeShape> (m_radius, m_height);
        else if(m_type == Type::Cylinder)
            m_collisionShape = std::make_shared <engine::app3D::CylinderShape> (m_size);
        else if(m_type == Type::Sphere)
            m_collisionShape = std::make_shared <engine::app3D::SphereShape> (m_radius);
        else if(m_type == Type::Plane)
            m_collisionShape = std::make_shared <engine::app3D::StaticPlaneShape> (m_planeNormal, m_planeConstant);
        else if(m_type == Type::ConvexHull || m_type == Type::BvhTriangleMesh) {
            auto &resourcesManager = Global::getCore().getDevice().getResourcesManager();
            const auto &points = resourcesManager.getMeshPoints(m_meshPath);

            if(m_type == Type::ConvexHull)
                m_collisionShape = std::make_shared <engine::app3D::ConvexHullShape> (points);
            else
                m_collisionShape = std::make_shared <engine::app3D::BvhTriangleMeshShape> (points);
        }
        else
            throw engine::Exception{"Collision shape type \"" + m_type.toString() + "\" not handled."};
    }
}

void CachedCollisionShapeDef::addPoofEffect(const engine::FloatVec3 &pos, const engine::FloatVec3 &rot) const
{
    auto &core = Global::getCore();
    auto &effectsPool = core.getEffectsPool();
    auto &defsCache = core.getDefsCache();

    auto rotatedOffset = engine::app3D::IrrlichtConversions::toVector(m_posOffset);
    irr::core::matrix4 mat;
    mat.setRotationDegrees(engine::app3D::IrrlichtConversions::toVector(rot));
    mat.rotateVect(rotatedOffset);

    const auto &posWithOffset = pos + engine::FloatVec3{rotatedOffset.X, rotatedOffset.Y, rotatedOffset.Z};

    // these don't have to be very accurate representations of actual physical bodies shapes

    if(m_type == Type::Box || m_type == Type::Cylinder)
        //effectsPool.add_boxZone(defsCache.Effect_Poof, posWithOffset, rot, m_size);
        effectsPool.add_sphereZone(defsCache.Effect_Poof, posWithOffset, rot, 1.4f * 0.5f * std::max({m_size.x, m_size.y, m_size.z}));
    else if(m_type == Type::Capsule || m_type == Type::Cone)
        effectsPool.add_boxZone(defsCache.Effect_Poof, posWithOffset, rot, {m_radius * 2.f, m_height, m_radius * 2.f});
        //effectsPool.add_sphereZone(defsCache.Effect_Poof, posWithOffset, rot, 0.5f * std::max({m_radius * 2.f, m_height}));
    else if(m_type == Type::Sphere)
        effectsPool.add_sphereZone(defsCache.Effect_Poof, posWithOffset, rot, m_radius);
}

const engine::app3D::CollisionShape &CachedCollisionShapeDef::getCollisionShape() const
{
    if(!m_collisionShape)
        throw engine::Exception{"Collision shape is nullptr."};

    return *m_collisionShape;
}

const std::shared_ptr <engine::app3D::CollisionShape> &CachedCollisionShapeDef::getCollisionShapePtr() const
{
    if(!m_collisionShape)
        throw engine::Exception{"Collision shape is nullptr."};

    return m_collisionShape;
}

const engine::FloatVec3 &CachedCollisionShapeDef::getPosOffset() const
{
    return m_posOffset;
}

float CachedCollisionShapeDef::getHeight() const
{
    // depending on shape type, there are different variables used

    if(m_type == Type::Sphere)
        return m_radius * 2.f;

    return std::max(m_height, m_size.y);
}

const CachedCollisionShapeDef::Type &CachedCollisionShapeDef::getType() const
{
    return m_type;
}

const engine::FloatVec3 &CachedCollisionShapeDef::getBoxSize() const
{
    if(m_type != Type::Box)
        throw engine::Exception{"Tried to get box size, but shape type is not Box."};

    return m_size;
}

} // namespace app
