#include "ItemFPPProperties.hpp"

#include "engine/app3D/defs/ModelDef.hpp"
#include "../../Global.hpp"
#include "../../Core.hpp"
#include "engine/util/DefDatabase.hpp"
#include "../AnimationFramesSetDef.hpp"

namespace app
{

void ItemFPPProperties::expose(engine::DataFile::Node &node)
{
    TRACK;

    node.var(m_modelDef_defName, "modelDef");
    node.var(m_animationFramesSetDef_defName, "animationFramesSetDef");
    node.var(m_basePosition, "basePosition");
    node.var(m_baseRotation, "baseRotation");
    node.var(m_aimPosition, "aimPosition");
    node.var(m_aimRotation, "aimRotation");

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        auto &defDatabase = Global::getCore().getDefDatabase();

        m_modelDef = defDatabase.getDef <engine::app3D::ModelDef> (m_modelDef_defName);
        m_animationFramesSetDef = defDatabase.getDef <AnimationFramesSetDef> (m_animationFramesSetDef_defName);
    }
}

bool ItemFPPProperties::hasModel() const
{
    return static_cast <bool> (m_modelDef);
}

bool ItemFPPProperties::hasAnimationFramesSetDef() const
{
    return static_cast <bool> (m_animationFramesSetDef);
}

const engine::app3D::ModelDef &ItemFPPProperties::getModelDef() const
{
    if(!m_modelDef)
        throw engine::Exception{"FPP model def is nullptr. This should have been checked before."};

    return *m_modelDef;
}

const AnimationFramesSetDef &ItemFPPProperties::getAnimationFramesSetDef() const
{
    if(!m_animationFramesSetDef)
        throw engine::Exception{"Animation frames set def is nullptr. This should have been checked before."};

    return *m_animationFramesSetDef;
}

const std::shared_ptr <engine::app3D::ModelDef> &ItemFPPProperties::getModelDefPtr() const
{
    if(!m_modelDef)
        throw engine::Exception{"FPP model def is nullptr. This should have been checked before."};

    return m_modelDef;
}

const std::shared_ptr <AnimationFramesSetDef> &ItemFPPProperties::getAnimationFramesSetDefPtr() const
{
    if(!m_animationFramesSetDef)
        throw engine::Exception{"Animation frames set def is nullptr. This should have been checked before."};

    return m_animationFramesSetDef;
}

const engine::FloatVec3 &ItemFPPProperties::getBasePosition() const
{
    return m_basePosition;
}

const engine::FloatVec3 &ItemFPPProperties::getBaseRotation() const
{
    return m_baseRotation;
}

const engine::FloatVec3 &ItemFPPProperties::getAimPosition() const
{
    return m_aimPosition;
}

const engine::FloatVec3 &ItemFPPProperties::getAimRotation() const
{
    return m_aimRotation;
}

} // namespace app
