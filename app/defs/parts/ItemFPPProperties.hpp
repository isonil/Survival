#ifndef APP_ITEM_FPP_PROPERTIES_HPP
#define APP_ITEM_FPP_PROPERTIES_HPP

#include "engine/util/DataFile.hpp"
#include "engine/util/Vec3.hpp"

namespace engine { namespace app3D { class ModelDef; } }

namespace app
{

class AnimationFramesSetDef;

class ItemFPPProperties : public engine::DataFile::Saveable
{
public:
    void expose(engine::DataFile::Node &node) override;

    bool hasModel() const;
    bool hasAnimationFramesSetDef() const;

    const engine::app3D::ModelDef &getModelDef() const;
    const AnimationFramesSetDef &getAnimationFramesSetDef() const;

    const std::shared_ptr <engine::app3D::ModelDef> &getModelDefPtr() const;
    const std::shared_ptr <AnimationFramesSetDef> &getAnimationFramesSetDefPtr() const;

    const engine::FloatVec3 &getBasePosition() const;
    const engine::FloatVec3 &getBaseRotation() const;
    const engine::FloatVec3 &getAimPosition() const;
    const engine::FloatVec3 &getAimRotation() const;

private:
    std::string m_modelDef_defName;
    std::string m_animationFramesSetDef_defName;
    std::shared_ptr <engine::app3D::ModelDef> m_modelDef;
    std::shared_ptr <AnimationFramesSetDef> m_animationFramesSetDef;
    engine::FloatVec3 m_basePosition;
    engine::FloatVec3 m_baseRotation;
    engine::FloatVec3 m_aimPosition;
    engine::FloatVec3 m_aimRotation;
};

} // namespace app

#endif // APP_ITEM_FPP_PROPERTIES_HPP
