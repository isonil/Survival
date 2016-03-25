#ifndef ENGINE_APP_3D_MODEL_HPP
#define ENGINE_APP_3D_MODEL_HPP

#include "../../util/Trace.hpp"
#include "../../util/Vec3.hpp"
#include "../../util/Range.hpp"
#include "../../util/Color.hpp"
#include "../defs/ModelDef.hpp"
#include "SceneNode.hpp"

#include <irrlicht/irrlicht.h>

#include <memory>
#include <string>
#include <functional>

namespace engine
{
namespace app3D
{

class Model : public SceneNode, public Tracked <Model>
{
public:
    Model(const std::shared_ptr <ModelDef> &modelDef, const std::weak_ptr <Device> &device, bool isFPP);

    void dropIrrObjects() override;
    void reloadIrrObjects() override;
    bool wantsEverUpdate() const override;
    void update(const FloatVec3 &cameraPos, const AppTime &appTime) override;

    void enableManualJointManipulation();
    bool hasAnyJointsCurrently() const;
    FloatVec3 getJointPosition(int index) const;
    FloatVec3 getJointRotation(int index) const;
    void setJointPosition(int index, const FloatVec3 &pos);
    void setJointRotation(int index, const FloatVec3 &rot);
    irr::scene::IBoneSceneNode *getJoint(const std::string &name) const;
    irr::scene::IBoneSceneNode *getJoint(int index) const;
    bool hasAnyIrrSceneNode() const;
    irr::scene::ISceneNode &getIrrSceneNode() const;

    void highlightNextFrame(const Color &color = k_defaultHighlightColor);
    void setPosition(const FloatVec3 &pos);
    void setRotation(const FloatVec3 &rot);
    void playAnimationLoop(const IntRange &range);
    void playSingleAnimationAndThenLoop(const IntRange &firstAnimationRange, const IntRange &loopedAnimationRange);
    void playSingleAnimation(const IntRange &range, const std::function <void()> &endCallback);
    const FloatVec3 &getPosition() const;
    const FloatVec3 &getRotation() const;
    ModelDef &getDef() const;

    ~Model() override;

private:
    class IrrlichtAnimationEndCallback : public irr::scene::IAnimationEndCallBack
    {
    public:
        IrrlichtAnimationEndCallback(Model &model);

        void OnAnimationEnd(irr::scene::IAnimatedMeshSceneNode *) override;

    private:
        Model &m_model;
    };

    friend class IrrlichtAnimationEndCallback;

    struct
    {
        irr::scene::IMeshSceneNode *meshNode{};
        irr::scene::IAnimatedMeshSceneNode *animatedMeshNode{};
        irr::scene::IBillboardSceneNode *billboardNode{};
        int LOD{-1};
        bool deferredRendering{};
        std::shared_ptr <int> batchedMeshIndex;
        std::shared_ptr <int> batchedBillboardIndex;
        float scale{1.f};
        float fromDistanceSq{-1.f};
        float toDistanceSq{-1.f};
        bool isBillboardOverlay{};
        bool useCenterAsOriginForBillboard{};
    } m_currentRender;

    enum class AnimationKind
    {
        None,
        SingleWithCallback,
        Loop,
        SingleAndThenLoop
    };

    int getCurrentAnimationFrame() const;
    void createRender(const ModelDef::LOD &LOD);
    void removeCurrentRender();
    void updateCurrentRenderPosition();
    void updateCurrentRenderRotation();
    void updateCurrentRenderMaterial();
    void updateCurrentRenderAnimation();

    static const Color k_defaultHighlightColor;

    std::shared_ptr <ModelDef> m_modelDef;
    FloatVec3 m_pos;
    FloatVec3 m_rot;
    IntRange m_animationFrameLoop;
    IntRange m_singleAnimationFrames;
    const bool m_isFPP;
    AnimationKind m_animationKind;
    IrrlichtAnimationEndCallback m_irrAnimationEndCallback;
    std::function <void()> m_onAnimationEnd;
    bool m_manualJointManipulation;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_MODEL_HPP
