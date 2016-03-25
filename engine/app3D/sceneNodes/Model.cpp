#include "Model.hpp"

#include "../../util/Exception.hpp"
#include "../../util/AppTime.hpp"
#include "../managers/ResourcesManager.hpp"
#include "../managers/SceneManager.hpp"
#include "../managers/ShadersManager.hpp"
#include "../Device.hpp"
#include "../IrrlichtConversions.hpp"

namespace engine
{
namespace app3D
{

Model::Model(const std::shared_ptr <ModelDef> &modelDef, const std::weak_ptr <Device> &device, bool isFPP)
    : SceneNode{device},
      m_currentRender{},
      m_modelDef{modelDef},
      m_isFPP{isFPP},
      m_animationKind{AnimationKind::None},
      m_irrAnimationEndCallback{*this},
      m_manualJointManipulation{}
{
    TRACK;

    if(!m_modelDef)
        throw Exception{"Model def is nullptr."};

    // force update to create render now
    update(getDevice_slow().getSceneManager().getCameraPosition(), {});
}

void Model::dropIrrObjects()
{
    TRACK;

    m_currentRender.meshNode = nullptr;
    m_currentRender.animatedMeshNode = nullptr;
    m_currentRender.billboardNode = nullptr;
    m_currentRender.LOD = -1;
    m_currentRender.deferredRendering = false;
    m_currentRender.batchedMeshIndex.reset();
    m_currentRender.batchedBillboardIndex.reset();
    m_currentRender.fromDistanceSq = -1.f;
    m_currentRender.toDistanceSq = -1.f;
    m_currentRender.isBillboardOverlay = false;
    m_currentRender.useCenterAsOriginForBillboard = false;
}

void Model::reloadIrrObjects()
{
    TRACK;

    m_currentRender.LOD = -1;
    m_currentRender.fromDistanceSq = -1.f;
    m_currentRender.toDistanceSq = -1.f;

    // force update to recreate render now
    update(getDevice_slow().getSceneManager().getCameraPosition(), {});
}

bool Model::wantsEverUpdate() const
{
    E_DASSERT(m_modelDef, "Model def is nullptr.");

    // if there is only one LOD, there is no point in updating Model each frame
    // (for this case, we have to make sure that constructor and reloadIrrObjects create renders)

    return m_modelDef->getLODs().size() != 1;
}

void Model::update(const FloatVec3 &cameraPos, const AppTime &appTime)
{
    TRACK;

    auto distanceSq = m_pos.getDistanceSq(cameraPos);

    // if distanceSq is in current render LOD distance range, then don't even bother querying for current LOD
    // (just a little optimization)
    if(distanceSq >= m_currentRender.fromDistanceSq && distanceSq <= m_currentRender.toDistanceSq)
        return;

    E_DASSERT(m_modelDef, "Model def is nullptr.");

    const auto &LOD = m_modelDef->getLOD(distanceSq);

    if(m_currentRender.LOD == LOD.getIndex())
        return;

    createRender(LOD);
}

void Model::enableManualJointManipulation()
{
    m_manualJointManipulation = true;

    if(m_currentRender.animatedMeshNode)
        m_currentRender.animatedMeshNode->setJointMode(irr::scene::EJUOR_CONTROL);
}

bool Model::hasAnyJointsCurrently() const
{
    if(m_currentRender.animatedMeshNode && m_currentRender.animatedMeshNode->getJointCount() > 0)
        return true;

    return false;
}

FloatVec3 Model::getJointPosition(int index) const
{
    if(m_currentRender.animatedMeshNode && index >= 0 && static_cast <size_t> (index) < m_currentRender.animatedMeshNode->getJointCount()) {
        const auto &pos = m_currentRender.animatedMeshNode->getJointNode(index)->getPosition();

        return {pos.X, pos.Y, pos.Z};
    }

    return m_pos;
}

FloatVec3 Model::getJointRotation(int index) const
{
    if(m_currentRender.animatedMeshNode && index >= 0 && static_cast <size_t> (index) < m_currentRender.animatedMeshNode->getJointCount()) {
        const auto &rot = m_currentRender.animatedMeshNode->getJointNode(index)->getRotation();

        return {rot.X, rot.Y, rot.Z};
    }

    return m_pos;
}

void Model::setJointPosition(int index, const FloatVec3 &pos)
{
    if(m_currentRender.animatedMeshNode && index >= 0 && static_cast <size_t> (index) < m_currentRender.animatedMeshNode->getJointCount()) {
        const auto &irrPos = IrrlichtConversions::toVector(pos);

        m_currentRender.animatedMeshNode->getJointNode(index)->setPosition(irrPos);
    }
}

void Model::setJointRotation(int index, const FloatVec3 &rot)
{
    if(m_currentRender.animatedMeshNode && index >= 0 && static_cast <size_t> (index) < m_currentRender.animatedMeshNode->getJointCount()) {
        const auto &irrRot = IrrlichtConversions::toVector(rot);

        m_currentRender.animatedMeshNode->getJointNode(index)->setRotation(irrRot);
    }
}

irr::scene::IBoneSceneNode *Model::getJoint(const std::string &name) const
{
    if(!m_currentRender.animatedMeshNode)
        return {};

    return m_currentRender.animatedMeshNode->getJointNode(name.c_str());
}

irr::scene::IBoneSceneNode *Model::getJoint(int index) const
{
    if(!m_currentRender.animatedMeshNode)
        return {};

    return m_currentRender.animatedMeshNode->getJointNode(index);
}

bool Model::hasAnyIrrSceneNode() const
{
    return m_currentRender.meshNode || m_currentRender.animatedMeshNode || m_currentRender.billboardNode;
}

irr::scene::ISceneNode &Model::getIrrSceneNode() const
{
    if(m_currentRender.meshNode)
        return *m_currentRender.meshNode;

    if(m_currentRender.animatedMeshNode)
        return *m_currentRender.animatedMeshNode;

    if(m_currentRender.billboardNode)
        return *m_currentRender.billboardNode;

    throw Exception{"There is no any irrlicht scene node. This should have been checked before."};
}

void Model::highlightNextFrame(const Color &color)
{
    E_DASSERT(m_modelDef, "Model def is nullptr.");

    const auto &LOD = m_modelDef->getLOD(0.f);

    if(LOD.hasIrrMesh()) {
        auto &device = getDevice_slow();

        device.getSceneManager().prepareToHighlightMesh(LOD.getIrrMesh(),
            getPosition(),
            getRotation(),
            LOD.getScale());

        device.getShadersManager().setOutlineShaderOutlineColor(color);
    }
}

void Model::setPosition(const FloatVec3 &pos)
{
    if(pos != m_pos) {
        m_pos = pos;
        updateCurrentRenderPosition();
    }
}

void Model::setRotation(const FloatVec3 &rot)
{
    if(rot != m_rot) {
        m_rot = rot;
        updateCurrentRenderRotation();
    }
}

void Model::playAnimationLoop(const IntRange &range)
{
    if(m_animationKind != AnimationKind::Loop || range != m_animationFrameLoop) {
        m_animationKind = AnimationKind::Loop;
        m_animationFrameLoop = range;

        updateCurrentRenderAnimation();
    }
}

void Model::playSingleAnimationAndThenLoop(const IntRange &firstAnimationRange, const IntRange &loopedAnimationRange)
{
    m_animationKind = AnimationKind::SingleAndThenLoop;
    m_singleAnimationFrames = firstAnimationRange;
    m_animationFrameLoop = loopedAnimationRange;

    updateCurrentRenderAnimation();
}

void Model::playSingleAnimation(const IntRange &range, const std::function <void()> &endCallback)
{
    m_animationKind = AnimationKind::SingleWithCallback;
    m_singleAnimationFrames = range;
    m_onAnimationEnd = endCallback;

    updateCurrentRenderAnimation();
}

const FloatVec3 &Model::getPosition() const
{
    return m_pos;
}

const FloatVec3 &Model::getRotation() const
{
    return m_rot;
}

ModelDef &Model::getDef() const
{
    E_DASSERT(m_modelDef, "Model def is nullptr.");
    return *m_modelDef;
}

Model::~Model()
{
    TRACK;

    if(!deviceExpired())
        removeCurrentRender();
}

Model::IrrlichtAnimationEndCallback::IrrlichtAnimationEndCallback(Model &model)
    : m_model{model}
{
}

void Model::IrrlichtAnimationEndCallback::OnAnimationEnd(irr::scene::IAnimatedMeshSceneNode *)
{
    if(!m_model.m_currentRender.animatedMeshNode)
        return;

    if(m_model.m_animationKind == AnimationKind::SingleWithCallback) {
        m_model.m_animationKind = AnimationKind::None;

        if(m_model.m_onAnimationEnd)
            m_model.m_onAnimationEnd(); // note that after calling this Model can no longer exist
    }
    else if(m_model.m_animationKind == AnimationKind::SingleAndThenLoop) {
        if(m_model.m_animationFrameLoop.isEmpty()) {
            m_model.m_animationKind = AnimationKind::None;
            m_model.m_currentRender.animatedMeshNode->setFrameLoop(0, 0);
        }
        else {
            m_model.m_animationKind = AnimationKind::Loop;
            m_model.m_currentRender.animatedMeshNode->setLoopMode(true);
            m_model.m_currentRender.animatedMeshNode->setFrameLoop(m_model.m_animationFrameLoop.from, m_model.m_animationFrameLoop.to);
        }
    }
}

int Model::getCurrentAnimationFrame() const
{
    if(m_currentRender.animatedMeshNode)
        return static_cast <int> (m_currentRender.animatedMeshNode->getFrameNr());

    return -1;
}

void Model::createRender(const ModelDef::LOD &LOD)
{
    TRACK;

    // in order to keep smooth transitions between animated LODs,
    // we have to keep current animation frame (if there is one)
    int currentAnimationFrame{getCurrentAnimationFrame()};

    removeCurrentRender();

    auto &device = getDevice_slow();

    m_currentRender.scale = LOD.getScale();
    m_currentRender.LOD = LOD.getIndex();
    m_currentRender.fromDistanceSq = LOD.getDistance() * LOD.getDistance();
    m_currentRender.toDistanceSq = LOD.getNextLODDistance() * LOD.getNextLODDistance();
    m_currentRender.isBillboardOverlay = LOD.isBillboardOverlay();
    m_currentRender.useCenterAsOriginForBillboard = LOD.getUseCenterAsOriginForBillboard();

    auto &irrSceneManager = *device.getIrrDevice().getSceneManager();
    auto &resourcesManager = device.getResourcesManager();
    auto &sceneManager = device.getSceneManager();

    const auto &scale = LOD.getScale();
    const auto &renderTechnique = LOD.getRenderTechnique();

    irr::scene::ISceneNode *parent{irrSceneManager.getRootSceneNode()};

    if(m_isFPP)
        parent = &sceneManager.getIrrCamera();

    if(renderTechnique == ModelDef::RenderTechnique::Mesh) {
        m_currentRender.meshNode = irrSceneManager.addMeshSceneNode(&LOD.getIrrMesh(), parent);

        if(!m_currentRender.meshNode)
            throw Exception{"Could not add mesh scene node."};

        if(!Math::fuzzyCompare(scale, 1.f)) {
            m_currentRender.meshNode->setScale({scale, scale, scale});
            m_currentRender.meshNode->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, true);
        }

        if(m_isFPP)
            m_currentRender.meshNode->setVisible(false);
    }
    else if(renderTechnique == ModelDef::RenderTechnique::AnimatedMesh) {
        m_currentRender.animatedMeshNode = irrSceneManager.addAnimatedMeshSceneNode(&LOD.getIrrAnimatedMesh(), parent);

        if(m_manualJointManipulation)
            m_currentRender.animatedMeshNode->setJointMode(irr::scene::EJUOR_CONTROL);

        m_currentRender.animatedMeshNode->setAnimationEndCallback(&m_irrAnimationEndCallback);

        if(!m_currentRender.animatedMeshNode)
            throw Exception{"Could not add animated mesh scene node."};

        if(!Math::fuzzyCompare(scale, 1.f)) {
            m_currentRender.animatedMeshNode->setScale({scale, scale, scale});
            m_currentRender.animatedMeshNode->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, true);
        }

        if(m_isFPP)
            m_currentRender.animatedMeshNode->setVisible(false);
    }
    else if(renderTechnique == ModelDef::RenderTechnique::Billboard) {
        m_currentRender.billboardNode = irrSceneManager.addBillboardSceneNode(parent,
            irr::core::dimension2df(scale, scale));

        if(!m_currentRender.billboardNode)
            throw Exception{"Could not add billboard scene node."};

        m_currentRender.billboardNode->setMaterialTexture(0, &LOD.getIrrBillboardTexture());

        if(LOD.isBillboardOverlay()) {
            m_currentRender.billboardNode->getMaterial(0).ZBuffer = irr::video::ECFN_NEVER;
            m_currentRender.billboardNode->getMaterial(0).ZWriteEnable = false;
        }

        if(m_isFPP)
            m_currentRender.billboardNode->setVisible(false);
    }
    else if(renderTechnique == ModelDef::RenderTechnique::MeshBatched) {
        auto &meshBatchManager = resourcesManager.getMeshBatchManager();

        m_currentRender.batchedMeshIndex = meshBatchManager.addMesh(LOD.getIrrMesh(), LOD.getBatchTag(), LOD.getForceAllUpNormalsWhenBatched());

        meshBatchManager.setMeshScale(*m_currentRender.batchedMeshIndex, {scale, scale, scale});

        if(m_isFPP) {
            E_WARNING("First person perspective nodes are not supported by batches.");
        }
    }
    else if(renderTechnique == ModelDef::RenderTechnique::BillboardBatched ||
            renderTechnique == ModelDef::RenderTechnique::HorizontalBillboardBatched) {
        auto &billboardBatchManager = resourcesManager.getBillboardBatchManager();

        bool horizontal{renderTechnique == ModelDef::RenderTechnique::HorizontalBillboardBatched};

        m_currentRender.batchedBillboardIndex = billboardBatchManager.addBillboard(LOD.getIrrBillboardTexture(), LOD.getBatchTag(), horizontal);

        billboardBatchManager.setBillboardScale(*m_currentRender.batchedBillboardIndex, {scale, scale});

        if(m_isFPP) {
            E_WARNING("First person perspective nodes are not supported by batches.");
        }

        if(LOD.isBillboardOverlay()) {
            E_WARNING("Billboard overlay not supported by batches.");
        }

        if(LOD.getUseCenterAsOriginForBillboard()) {
            E_WARNING("Using center as origin not supported by batches.");
        }
    }

    updateCurrentRenderPosition();
    updateCurrentRenderRotation();
    updateCurrentRenderMaterial();
    updateCurrentRenderAnimation();

    // set current animation frame (if applicable)
    if(currentAnimationFrame >= 0) {
        if(m_currentRender.animatedMeshNode) {
            auto *mesh = m_currentRender.animatedMeshNode->getMesh();

            if(mesh && currentAnimationFrame < static_cast <int> (mesh->getFrameCount()))
                m_currentRender.animatedMeshNode->setCurrentFrame(currentAnimationFrame);
        }
        else {
            // if LOD has changed, but there is no animation in this LOD,
            // then we have to check if we should invoke animation end callback

            if(m_animationKind == AnimationKind::SingleWithCallback) {
                m_animationKind = AnimationKind::None;

                if(m_onAnimationEnd)
                    m_onAnimationEnd(); // note that after calling this Model can no longer exist
            }

            // we don't care about other animation kinds (they can just replay when
            // LOD with animation is used again)
        }
    }
}

void Model::removeCurrentRender()
{
    TRACK;

    if(m_currentRender.meshNode) {
        m_currentRender.meshNode->remove();
        m_currentRender.meshNode = nullptr;
    }

    if(m_currentRender.animatedMeshNode) {
        m_currentRender.animatedMeshNode->remove();
        m_currentRender.animatedMeshNode = nullptr;
    }

    if(m_currentRender.billboardNode) {
        m_currentRender.billboardNode->remove();
        m_currentRender.billboardNode = nullptr;
    }

    if(m_currentRender.batchedMeshIndex) {
        getDevice_slow().getResourcesManager().getMeshBatchManager().removeMesh(*m_currentRender.batchedMeshIndex);
        m_currentRender.batchedMeshIndex.reset();
    }

    if(m_currentRender.batchedBillboardIndex) {
        getDevice_slow().getResourcesManager().getBillboardBatchManager().removeBillboard(*m_currentRender.batchedBillboardIndex);
        m_currentRender.batchedBillboardIndex.reset();
    }

    m_currentRender.LOD = -1;
    m_currentRender.deferredRendering = false;
    m_currentRender.scale = 1.f;
    m_currentRender.fromDistanceSq = -1.f;
    m_currentRender.toDistanceSq = -1.f;
    m_currentRender.isBillboardOverlay = false;
    m_currentRender.useCenterAsOriginForBillboard = false;
}

void Model::updateCurrentRenderPosition()
{
    TRACK;

    auto &meshNode = m_currentRender.meshNode;
    auto &animatedMeshNode = m_currentRender.animatedMeshNode;
    auto &billboardNode = m_currentRender.billboardNode;

    if(meshNode) {
        const auto &irrPos = IrrlichtConversions::toVector(m_pos);
        meshNode->setPosition(irrPos);
    }

    if(animatedMeshNode) {
        const auto &irrPos = IrrlichtConversions::toVector(m_pos);
        animatedMeshNode->setPosition(irrPos);
    }

    if(billboardNode) {
        irr::core::vector3df irrPos;

        if(m_currentRender.useCenterAsOriginForBillboard)
            irrPos = IrrlichtConversions::toVector(m_pos);
        else
            irrPos = IrrlichtConversions::toVector(m_pos.movedY(m_currentRender.scale * 0.5f));

        billboardNode->setPosition(irrPos);
    }

    if(m_currentRender.batchedMeshIndex) {
        auto &meshBatchManager = getDevice_slow().getResourcesManager().getMeshBatchManager();
        meshBatchManager.setMeshPosition(*m_currentRender.batchedMeshIndex, m_pos);
    }

    if(m_currentRender.batchedBillboardIndex) {
        auto &billboardBatchManager = getDevice_slow().getResourcesManager().getBillboardBatchManager();
        billboardBatchManager.setBillboardPosition(*m_currentRender.batchedBillboardIndex, m_pos.movedY(m_currentRender.scale * 0.5f));
    }
}

void Model::updateCurrentRenderRotation()
{
    TRACK;

    auto &meshNode = m_currentRender.meshNode;
    auto &animatedMeshNode = m_currentRender.animatedMeshNode;

    if(meshNode) {
        const auto &irrRot = IrrlichtConversions::toVector(m_rot);
        meshNode->setRotation(irrRot);
    }

    if(animatedMeshNode) {
        const auto &irrRot = IrrlichtConversions::toVector(m_rot);
        animatedMeshNode->setRotation(irrRot);
    }

    if(m_currentRender.batchedMeshIndex) {
        auto &meshBatchManager = getDevice_slow().getResourcesManager().getMeshBatchManager();
        meshBatchManager.setMeshRotation(*m_currentRender.batchedMeshIndex, m_rot);
    }
}

void Model::updateCurrentRenderMaterial()
{
    TRACK;

    // TODO

    auto &device = getDevice_slow();
    bool shouldUseDeferred{device.isUsingDeferredRendering()};

    if(shouldUseDeferred) {
        m_currentRender.deferredRendering = true;

        if(m_currentRender.meshNode) {
            //m_currentRender.meshNode->setMaterialType(Shaders::getDefaultDeferredRenderingMaterial());
            m_currentRender.meshNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
            m_currentRender.meshNode->setMaterialFlag(irr::video::EMF_FOG_ENABLE, false);
        }

        if(m_currentRender.animatedMeshNode) {
            //m_currentRender.animatedMeshNode->setMaterialType(Shaders::getDefaultDeferredRenderingMaterial());
            m_currentRender.animatedMeshNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
            m_currentRender.animatedMeshNode->setMaterialFlag(irr::video::EMF_FOG_ENABLE, false);
        }

        if(m_currentRender.billboardNode) {
            //m_currentRender.billboardNode->setMaterialType(Shaders::getDefaultDeferredRenderingMaterial());
            m_currentRender.billboardNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
            m_currentRender.billboardNode->setMaterialFlag(irr::video::EMF_FOG_ENABLE, false);
        }
    }
    else {
        m_currentRender.deferredRendering = false;

        if(m_currentRender.meshNode) {
            //m_currentRender.meshNode->setMaterialType(Shaders::getDefaultMaterial());
            m_currentRender.meshNode->setMaterialFlag(irr::video::EMF_LIGHTING, true);
            m_currentRender.meshNode->setMaterialFlag(irr::video::EMF_FOG_ENABLE, true);
        }

        if(m_currentRender.animatedMeshNode) {
            //m_currentRender.animatedMeshNode->setMaterialType(Shaders::getDefaultMaterial());
            m_currentRender.animatedMeshNode->setMaterialFlag(irr::video::EMF_LIGHTING, true);
            m_currentRender.animatedMeshNode->setMaterialFlag(irr::video::EMF_FOG_ENABLE, true);
        }

        if(m_currentRender.billboardNode) {
            //m_currentRender.billboardNode->setMaterialType(irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL); // Shaders::getDefaultMaterial()

            if(m_currentRender.isBillboardOverlay) {
                m_currentRender.billboardNode->setMaterialType(irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL);
                m_currentRender.billboardNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
                m_currentRender.billboardNode->setMaterialFlag(irr::video::EMF_FOG_ENABLE, false);
            }
            else {
                m_currentRender.billboardNode->setMaterialFlag(irr::video::EMF_LIGHTING, true);
                m_currentRender.billboardNode->setMaterialFlag(irr::video::EMF_FOG_ENABLE, true);
            }
        }
    }
}

void Model::updateCurrentRenderAnimation()
{
    TRACK;

    if(m_currentRender.animatedMeshNode) {
        if(m_animationKind == AnimationKind::None) {
            m_currentRender.animatedMeshNode->setLoopMode(false);
            m_currentRender.animatedMeshNode->setFrameLoop(0, 0);
        }
        else if(m_animationKind == AnimationKind::SingleWithCallback) {
            m_currentRender.animatedMeshNode->setLoopMode(false);

            if(m_singleAnimationFrames.isEmpty()) {
                m_animationKind = AnimationKind::None;
                m_currentRender.animatedMeshNode->setFrameLoop(0, 0);

                if(m_onAnimationEnd)
                    m_onAnimationEnd(); // note that after calling this Model can no longer exist
            }
            else
                m_currentRender.animatedMeshNode->setFrameLoop(m_singleAnimationFrames.from, m_singleAnimationFrames.to);
        }
        else if(m_animationKind == AnimationKind::SingleAndThenLoop) {
            m_currentRender.animatedMeshNode->setLoopMode(false);

            if(m_singleAnimationFrames.isEmpty()) {
                if(m_animationFrameLoop.isEmpty()) {
                    m_animationKind = AnimationKind::None;
                    m_currentRender.animatedMeshNode->setFrameLoop(0, 0);
                }
                else {
                    m_animationKind = AnimationKind::Loop;
                    m_currentRender.animatedMeshNode->setLoopMode(true);
                    m_currentRender.animatedMeshNode->setFrameLoop(m_animationFrameLoop.from, m_animationFrameLoop.to);
                }
            }
            else
                m_currentRender.animatedMeshNode->setFrameLoop(m_singleAnimationFrames.from, m_singleAnimationFrames.to);

        }
        else if(m_animationKind == AnimationKind::Loop) {
            m_currentRender.animatedMeshNode->setLoopMode(true);

            if(m_animationFrameLoop.isEmpty()) {
                m_animationKind = AnimationKind::None;
                m_currentRender.animatedMeshNode->setLoopMode(false);
                m_currentRender.animatedMeshNode->setFrameLoop(0, 0);
            }
            else
                m_currentRender.animatedMeshNode->setFrameLoop(m_animationFrameLoop.from, m_animationFrameLoop.to);
        }
    }
    else if(m_animationKind == AnimationKind::SingleWithCallback) {
        m_animationKind = AnimationKind::None;

        if(m_onAnimationEnd)
            m_onAnimationEnd();
    }
}

const Color Model::k_defaultHighlightColor{0.3f, 0.3f, 0.8f};

} // namespace app3D
} // namespace engine
