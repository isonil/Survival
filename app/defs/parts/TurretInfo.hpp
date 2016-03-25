#ifndef APP_TURRET_INFO_HPP
#define APP_TURRET_INFO_HPP

#include "engine/util/DataFile.hpp"
#include "engine/util/Vec3.hpp"

namespace engine { namespace app3D { class ModelDef; } }

namespace app
{

class CachedCollisionShapeDef;
class ItemDef;

class TurretInfo : public engine::DataFile::Saveable
{
public:
    TurretInfo();

    void expose(engine::DataFile::Node &node) override;

    bool isTurret() const;

    CachedCollisionShapeDef &getHeadCachedCollisionShapeDef() const;
    const std::shared_ptr <CachedCollisionShapeDef> &getHeadCachedCollisionShapeDefPtr() const;

    engine::app3D::ModelDef &getHeadModelDef() const;
    const std::shared_ptr <engine::app3D::ModelDef> &getHeadModelDefPtr() const;

    ItemDef &getWeaponItemDef() const;
    const std::shared_ptr <ItemDef> &getWeaponItemDefPtr() const;

    engine::FloatVec3 getRandomBarrelOffset() const;
    engine::FloatVec3 getRandomBarrelOffsetRotated(const engine::FloatVec3 &rotation) const;

    float getDistanceToHead() const;

private:
    bool m_isTurret;

    std::string m_headCachedCollisionShapeDef_defName;
    std::shared_ptr <CachedCollisionShapeDef> m_headCachedCollisionShapeDef;

    std::string m_headModelDef_defName;
    std::shared_ptr <engine::app3D::ModelDef> m_headModelDef;

    std::string m_weaponItemDef_defName;
    std::shared_ptr <ItemDef> m_weaponItemDef;

    std::vector <engine::FloatVec3> m_barrelOffsets;

    float m_distanceToHead;
};

} // namespace app

#endif // APP_TURRET_INFO_HPP

