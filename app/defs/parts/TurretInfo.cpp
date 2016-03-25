#include "TurretInfo.hpp"

#include "engine/app3D/defs/ModelDef.hpp"
#include "../../Global.hpp"
#include "../../Core.hpp"
#include "engine/app3D/IrrlichtConversions.hpp"
#include "engine/util/DefDatabase.hpp"
#include "../CachedCollisionShapeDef.hpp"
#include "../ItemDef.hpp"

namespace app
{

TurretInfo::TurretInfo()
    : m_isTurret{},
      m_distanceToHead{}
{
}

void TurretInfo::expose(engine::DataFile::Node &node)
{
    node.var(m_headCachedCollisionShapeDef_defName, "headCachedCollisionShapeDef");
    node.var(m_headModelDef_defName, "headModelDef");
    node.var(m_weaponItemDef_defName, "weaponItemDef");
    node.var(m_distanceToHead, "distanceToHead");
    node.var(m_barrelOffsets, "barrelOffsets");

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        auto &defDatabase = Global::getCore().getDefDatabase();

        m_isTurret = true;

        m_headCachedCollisionShapeDef = defDatabase.getDef <CachedCollisionShapeDef> (m_headCachedCollisionShapeDef_defName);
        m_headModelDef = defDatabase.getDef <engine::app3D::ModelDef> (m_headModelDef_defName);
        m_weaponItemDef = defDatabase.getDef <ItemDef> (m_weaponItemDef_defName);

        if(m_barrelOffsets.empty())
            throw engine::Exception{"There must be at least one barrel offset."};
    }
}

bool TurretInfo::isTurret() const
{
    return m_isTurret;
}

CachedCollisionShapeDef &TurretInfo::getHeadCachedCollisionShapeDef() const
{
    if(!m_headCachedCollisionShapeDef)
        throw engine::Exception{"Cached collision shape def is nullptr."};

    return *m_headCachedCollisionShapeDef;
}

const std::shared_ptr <CachedCollisionShapeDef> &TurretInfo::getHeadCachedCollisionShapeDefPtr() const
{
    if(!m_headCachedCollisionShapeDef)
        throw engine::Exception{"Cached collision shape def is nullptr."};

    return m_headCachedCollisionShapeDef;
}

engine::app3D::ModelDef &TurretInfo::getHeadModelDef() const
{
    if(!m_headModelDef)
        throw engine::Exception{"Model def is nullptr."};

    return *m_headModelDef;
}

const std::shared_ptr <engine::app3D::ModelDef> &TurretInfo::getHeadModelDefPtr() const
{
    if(!m_headModelDef)
        throw engine::Exception{"Model def is nullptr."};

    return m_headModelDef;
}

ItemDef &TurretInfo::getWeaponItemDef() const
{
    if(!m_weaponItemDef)
        throw engine::Exception{"Item def is nullptr."};

    return *m_weaponItemDef;
}

const std::shared_ptr <ItemDef> &TurretInfo::getWeaponItemDefPtr() const
{
    if(!m_weaponItemDef)
        throw engine::Exception{"Item def is nullptr."};

    return m_weaponItemDef;
}

engine::FloatVec3 TurretInfo::getRandomBarrelOffset() const
{
    if(m_barrelOffsets.empty())
        throw engine::Exception{"No barrel offsets."};

    return m_barrelOffsets[engine::Random::rangeExclusive(0, m_barrelOffsets.size())];
}

engine::FloatVec3 TurretInfo::getRandomBarrelOffsetRotated(const engine::FloatVec3 &rotation) const
{
    auto irrVec = engine::app3D::IrrlichtConversions::toVector(getRandomBarrelOffset());

    irr::core::matrix4 mat;
    mat.setRotationDegrees(engine::app3D::IrrlichtConversions::toVector(rotation));
    mat.rotateVect(irrVec);

    return {irrVec.X, irrVec.Y, irrVec.Z};
}

float TurretInfo::getDistanceToHead() const
{
    return m_distanceToHead;
}

} // namespace app
