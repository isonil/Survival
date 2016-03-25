#include "SceneManager.hpp"

#include "../../util/Exception.hpp"
#include "../managers/ShadersManager.hpp"
#include "../sceneNodes/SceneNode.hpp"
#include "../sceneNodes/Model.hpp"
#include "../sceneNodes/Terrain.hpp"
#include "../sceneNodes/Water.hpp"
#include "../sceneNodes/Light.hpp"
#include "../sceneNodes/Island.hpp"
#include "../Device.hpp"
#include "../IrrlichtConversions.hpp"
#include "../IslandGenerator.hpp"
#include "ResourcesManager.hpp"

namespace engine
{
namespace app3D
{

SceneManager::SceneManager(Device &device)
    : m_device{device},
      m_irrCamera{},
      m_irrSkyBox{},
      m_highlightBufferRenderTarget{},
      m_ambientLight{1.f},
      m_FOVMultiplier{1.f}
{
    reloadIrrObjects();
    initFullScreenQuad();

    m_lineMaterial.MaterialType = irr::video::EMT_TRANSPARENT_ADD_COLOR;
    m_lineMaterial.Lighting = false;
    m_lineMaterial.ZBuffer = irr::video::ECFN_NEVER;
    m_lineMaterial.ZWriteEnable = false;
    m_lineMaterial.FogEnable = false;
}

void SceneManager::setAmbientLight(float intensity)
{
    m_ambientLight = intensity;
    irr::video::SColorf irrColor{intensity, intensity, intensity};
    m_device.getIrrDevice().getSceneManager()->setAmbientLight(irrColor);
}

irr::scene::ICameraSceneNode &SceneManager::getIrrCamera()
{
    if(!m_irrCamera)
        throw Exception{"Irrlicht camera is nullptr."};

    return *m_irrCamera;
}

void SceneManager::setCameraPosition(const FloatVec3 &pos)
{
    if(!m_irrCamera)
        throw Exception{"Irrlicht camera is nullptr."};

    const auto &irrPos = IrrlichtConversions::toVector(pos);
    m_irrCamera->setPosition(irrPos);
}

FloatVec3 SceneManager::getCameraPosition() const
{
    if(!m_irrCamera)
        throw Exception{"Irrlicht camera is nullptr."};

    const auto &pos = m_irrCamera->getPosition();
    return FloatVec3(pos.X, pos.Y, pos.Z);
}

FloatVec3 SceneManager::getCameraRotation() const
{
    if(!m_irrCamera)
        throw Exception{"Irrlicht camera is nullptr."};

    const auto &rot = m_irrCamera->getRotation();
    return FloatVec3(rot.X, rot.Y, rot.Z);
}

FloatVec3 SceneManager::getCameraLookVec() const
{
    if(!m_irrCamera)
        throw Exception{"Irrlicht camera is nullptr."};

    auto vec = m_irrCamera->getTarget() - m_irrCamera->getPosition();
    vec.normalize();

    return FloatVec3(vec.X, vec.Y, vec.Z);
}

float SceneManager::getAmbientLight() const
{
    return m_ambientLight;
}

void SceneManager::setFOVMultiplier(float multiplier)
{
    m_FOVMultiplier = multiplier;

    E_DASSERT(m_irrCamera, "Irrlicht camera is nullptr.");

    m_irrCamera->setFOV(Math::degToRad(m_device.getVideoSettings().FOV * m_FOVMultiplier));
}

float SceneManager::getFOV() const
{
    E_DASSERT(m_irrCamera, "Irrlicht camera is nullptr.");

    return m_irrCamera->getFOV();
}

void SceneManager::dropIrrObjects()
{
    for(auto &elem : m_sceneNodes_wantUpdate) {
        E_DASSERT(elem, "Scene node is nullptr.");
        elem->dropIrrObjects();
    }

    for(auto &elem : m_sceneNodes_dontWantUpdate) {
        E_DASSERT(elem, "Scene node is nullptr.");
        elem->dropIrrObjects();
    }

    if(m_highlightedNode.sceneNode_rendered) {
        m_highlightedNode.sceneNode_rendered->remove();
        m_highlightedNode.sceneNode_rendered = nullptr;
    }

    if(m_irrCamera) {
        m_irrCamera->remove();
        m_irrCamera = nullptr;
    }

    if(m_irrSkyBox) {
        m_irrSkyBox->remove();
        m_irrSkyBox = nullptr;
    }

    m_highlightBufferRenderTarget = nullptr;
}

void SceneManager::reloadIrrObjects()
{
    auto &irrDevice = m_device.getIrrDevice();
    auto &sceneManager = *irrDevice.getSceneManager();
    auto &driver = *irrDevice.getVideoDriver();

    m_irrCamera = sceneManager.addCameraSceneNodeFPS(nullptr, k_cameraRotateSpeed, 0.102f); // 0.102f

    if(!m_irrCamera)
        throw Exception{"Could not create camera scene node."};

    m_irrCamera->setFOV(Math::degToRad(m_device.getVideoSettings().FOV * m_FOVMultiplier));
    m_irrCamera->setNearValue(k_cameraNearValue);
    m_irrCamera->setFarValue(k_cameraFarValue);

    ResourcesManager &resourcesManager = m_device.getResourcesManager();

    auto &up = resourcesManager.loadIrrTexture(k_skyBoxTexturesPath + "up.png", false);
    auto &down = resourcesManager.loadIrrTexture(k_skyBoxTexturesPath + "down.png", false);
    auto &left = resourcesManager.loadIrrTexture(k_skyBoxTexturesPath + "left.png", false);
    auto &right = resourcesManager.loadIrrTexture(k_skyBoxTexturesPath + "right.png", false);
    auto &front = resourcesManager.loadIrrTexture(k_skyBoxTexturesPath + "front.png", false);
    auto &back = resourcesManager.loadIrrTexture(k_skyBoxTexturesPath + "back.png", false);

    m_irrSkyBox = sceneManager.addSkyBoxSceneNode(&up, &down, &left, &right, &front, &back);

    if(!m_irrSkyBox)
        throw Exception{"Could not create skybox scene node."};

    ShadersManager &shadersManager = m_device.getShadersManager();

    m_irrSkyBox->setMaterialType(shadersManager.getSkyShaderMaterialType());

    m_highlightBufferRenderTarget = driver.addRenderTargetTexture(driver.getScreenSize());

    if(!m_highlightBufferRenderTarget)
        throw Exception{"Could not create highlight buffer render target."};

    for(auto &elem : m_sceneNodes_wantUpdate) {
        E_DASSERT(elem, "Scene node is nullptr.");
        elem->reloadIrrObjects();
    }

    for(auto &elem : m_sceneNodes_dontWantUpdate) {
        E_DASSERT(elem, "Scene node is nullptr.");
        elem->reloadIrrObjects();
    }

    setAmbientLight(m_ambientLight);
}

void SceneManager::update(const AppTime &appTime)
{
    TRACK;

    const auto &irrCameraPos = m_device.getSceneManager().getIrrCamera().getPosition();
    FloatVec3 cameraPos{irrCameraPos.X, irrCameraPos.Y, irrCameraPos.Z};

    for(size_t i = 0; i < m_sceneNodes_wantUpdate.size();) {
        E_DASSERT(m_sceneNodes_wantUpdate[i], "Scene node is nullptr.");

        if(m_sceneNodes_wantUpdate[i].unique()) {
            std::swap(m_sceneNodes_wantUpdate[i], m_sceneNodes_wantUpdate.back());
            m_sceneNodes_wantUpdate.pop_back();
        }
        else {
            m_sceneNodes_wantUpdate[i]->update(cameraPos, appTime);
            ++i;
        }
    }

    for(size_t i = 0; i < m_sceneNodes_dontWantUpdate.size();) {
        E_DASSERT(m_sceneNodes_dontWantUpdate[i], "Scene node is nullptr.");

        if(m_sceneNodes_dontWantUpdate[i].unique()) {
            std::swap(m_sceneNodes_dontWantUpdate[i], m_sceneNodes_dontWantUpdate.back());
            m_sceneNodes_dontWantUpdate.pop_back();
        }
        else
            ++i;
    }
}

void SceneManager::drawHighlightedNode()
{
    auto &irrDevice = m_device.getIrrDevice();

    if(m_highlightedNode.sceneNode_rendered && m_highlightedNode.sceneNode_rendered->getMesh() != m_highlightedNode.mesh_toRender) {
        // we're currently rendering highlighted node, but we want to render
        // different mesh (or don't want to render anything), so we have to remove
        // current render

        m_highlightedNode.sceneNode_rendered->remove();
        m_highlightedNode.sceneNode_rendered = nullptr;
    }

    if(!m_highlightedNode.sceneNode_rendered && m_highlightedNode.mesh_toRender) {
        // we want to render something, but we don't render anything currently

        m_highlightedNode.sceneNode_rendered = irrDevice.getSceneManager()->addMeshSceneNode(m_highlightedNode.mesh_toRender);

        if(!m_highlightedNode.sceneNode_rendered)
            throw Exception{"Could not create highlighted scene node."};

        m_highlightedNode.sceneNode_rendered->setVisible(false);
        m_highlightedNode.sceneNode_rendered->setMaterialType(m_device.getShadersManager().getWhiteShaderMaterialType());
        m_highlightedNode.sceneNode_rendered->setMaterialFlag(irr::video::EMF_LIGHTING, false);
        m_highlightedNode.sceneNode_rendered->setMaterialFlag(irr::video::EMF_FOG_ENABLE, false);
    }

    if(m_highlightedNode.sceneNode_rendered) {
        // we're currently rendering highlighted node

        float scale{m_highlightedNode.scale_toRender};

        const auto &irrPos = IrrlichtConversions::toVector(m_highlightedNode.pos_toRender);
        const auto &irrRot = IrrlichtConversions::toVector(m_highlightedNode.rot_toRender);
        irr::core::vector3df irrScale{scale, scale, scale};

        if(m_highlightedNode.sceneNode_rendered->getPosition() != irrPos)
            m_highlightedNode.sceneNode_rendered->setPosition(irrPos);

        if(m_highlightedNode.sceneNode_rendered->getRotation() != irrRot)
            m_highlightedNode.sceneNode_rendered->setRotation(irrRot);

        if(m_highlightedNode.sceneNode_rendered->getScale() != irrScale)
            m_highlightedNode.sceneNode_rendered->setScale(irrScale);

        auto &videoDriver = *irrDevice.getVideoDriver();

        E_DASSERT(m_highlightBufferRenderTarget, "Highlight buffer render target is nullptr.");

        videoDriver.setRenderTarget(m_highlightBufferRenderTarget, true, true);

        m_highlightedNode.sceneNode_rendered->setVisible(true);
        m_highlightedNode.sceneNode_rendered->updateAbsolutePosition();
        m_highlightedNode.sceneNode_rendered->render();
        m_highlightedNode.sceneNode_rendered->setVisible(false);

        videoDriver.setRenderTarget(nullptr, false, false);

        drawFullScreenQuad(m_device.getShadersManager().getOutlineShaderMaterialType(), *m_highlightBufferRenderTarget);
    }

    // rendered, so we can set mesh_toRender to nullptr and wait until next frame
    m_highlightedNode.mesh_toRender = nullptr;
}

void SceneManager::drawFPPModels()
{
    for(size_t i = 0; i < m_FPPModels.size();) {
        const auto &shared = m_FPPModels[i].lock();

        if(shared) {
            if(shared->hasAnyIrrSceneNode()) {
                auto &node = shared->getIrrSceneNode();

                node.setVisible(true);
                node.updateAbsolutePosition();
                node.render();
                node.setVisible(false);
            }

            ++i;
        }
        else {
            std::swap(m_FPPModels[i], m_FPPModels.back());
            m_FPPModels.pop_back();
        }
    }
}

void SceneManager::prepareToHighlightMesh(irr::scene::IMesh &mesh, const FloatVec3 &pos, const FloatVec3 &rot, float scale)
{
    m_highlightedNode.mesh_toRender = &mesh;
    m_highlightedNode.pos_toRender = pos;
    m_highlightedNode.rot_toRender = rot;
    m_highlightedNode.scale_toRender = scale;
}

void SceneManager::setLineMaterial()
{
    auto &videoDriver = *m_device.getIrrDevice().getVideoDriver();

    videoDriver.setMaterial(m_lineMaterial);
    videoDriver.setTransform(irr::video::ETS_WORLD, irr::core::IdentityMatrix);
}

void SceneManager::draw3DLine(const FloatVec3 &from, const FloatVec3 &to, const Color &color) const
{
    // line material must be set before

    auto &videoDriver = *m_device.getIrrDevice().getVideoDriver();

    videoDriver.draw3DLine(IrrlichtConversions::toVector(from),
                           IrrlichtConversions::toVector(to),
                           IrrlichtConversions::toColor(color));
}

std::shared_ptr <Model> SceneManager::addModel(const std::shared_ptr <ModelDef> &modelDef, bool isFPP)
{
    TRACK;

    if(!modelDef)
        throw Exception{"Model def is nullptr."};

    const auto &sceneNode = std::make_shared <Model> (modelDef, m_device.getPtr(), isFPP);
    addSceneNode(sceneNode);

    if(isFPP)
        m_FPPModels.push_back(sceneNode);

    return sceneNode;
}

std::shared_ptr <Terrain> SceneManager::addTerrain(const std::shared_ptr <TerrainDef> &terrainDef)
{
    TRACK;

    if(!terrainDef)
        throw Exception{"Terrain def is nullptr."};

    const auto &sceneNode = std::make_shared <Terrain> (terrainDef, m_device.getPtr());
    addSceneNode(sceneNode);
    return sceneNode;
}

std::shared_ptr <Water> SceneManager::addWater(const std::shared_ptr <TerrainDef> &terrainDef)
{
    TRACK;

    if(!terrainDef)
        throw Exception{"Terrain def is nullptr."};

    const auto &sceneNode = std::make_shared <Water> (terrainDef, m_device.getPtr());
    addSceneNode(sceneNode);
    return sceneNode;
}

std::shared_ptr <Light> SceneManager::addLight(const std::shared_ptr <LightDef> &lightDef)
{
    TRACK;

    if(!lightDef)
        throw Exception{"Light def is nullptr."};

    const auto &sceneNode = std::make_shared <Light> (lightDef, m_device.getPtr());
    addSceneNode(sceneNode);
    return sceneNode;
}

std::shared_ptr <Island> SceneManager::addIsland()
{
    TRACK;

    IslandGenerator generator;

    const auto &sceneNode = generator.generateIsland(m_device.getPtr());
    addSceneNode(sceneNode);
    return sceneNode;
}

const float SceneManager::k_cameraFarValue{500.f};

void SceneManager::initFullScreenQuad()
{
    TRACK;

    m_fullScreenQuad.material.TextureLayer[0].TextureWrapU = irr::video::ETC_CLAMP_TO_EDGE;
    m_fullScreenQuad.material.TextureLayer[0].TextureWrapV = irr::video::ETC_CLAMP_TO_EDGE;
    m_fullScreenQuad.material.ZBuffer = irr::video::ECFN_NEVER;
    m_fullScreenQuad.material.ZWriteEnable = false;

    m_fullScreenQuad.vertices[0] = irr::video::S3DVertex{-1, 1, 0, 0, 0, 0, irr::video::SColor{255, 255, 255, 255}, 0, 1};
    m_fullScreenQuad.vertices[1] = irr::video::S3DVertex{1, 1, 0, 0, 0, 0, irr::video::SColor{255, 255, 255, 255}, 1, 1};
    m_fullScreenQuad.vertices[2] = irr::video::S3DVertex{1, -1, 0, 0, 0, 0, irr::video::SColor{255, 255, 255, 255}, 1, 0};
    m_fullScreenQuad.vertices[3] = irr::video::S3DVertex{-1, -1, 0, 0, 0, 0, irr::video::SColor{255, 255, 255, 255}, 0, 0};

    m_fullScreenQuad.indices[0] = 0;
    m_fullScreenQuad.indices[1] = 1;
    m_fullScreenQuad.indices[2] = 3;
    m_fullScreenQuad.indices[3] = 1;
    m_fullScreenQuad.indices[4] = 2;
    m_fullScreenQuad.indices[5] = 3;
}

void SceneManager::addSceneNode(const std::shared_ptr <SceneNode> &sceneNode)
{
    TRACK;

    if(!sceneNode)
        throw Exception{"Tried to add nullptr scene node."};

    if(sceneNode->wantsEverUpdate())
        m_sceneNodes_wantUpdate.push_back(sceneNode);
    else
        m_sceneNodes_dontWantUpdate.push_back(sceneNode);
}

void SceneManager::drawFullScreenQuad(irr::video::E_MATERIAL_TYPE materialType, irr::video::ITexture &texture)
{
    TRACK;

    m_fullScreenQuad.material.MaterialType = materialType;
    m_fullScreenQuad.material.TextureLayer[0].Texture = &texture;

    auto &irrDriver = *m_device.getIrrDevice().getVideoDriver();

    irrDriver.setMaterial(m_fullScreenQuad.material);
    irrDriver.setTransform(irr::video::ETS_PROJECTION, irr::core::IdentityMatrix);
    irrDriver.setTransform(irr::video::ETS_VIEW, irr::core::IdentityMatrix);
    irrDriver.setTransform(irr::video::ETS_WORLD, irr::core::IdentityMatrix);
    irrDriver.drawIndexedTriangleList(&m_fullScreenQuad.vertices[0], 4, &m_fullScreenQuad.indices[0], 2);
}

const float SceneManager::k_cameraRotateSpeed{100.f};
const float SceneManager::k_cameraNearValue{0.05f};
const std::string SceneManager::k_skyBoxTexturesPath = "sky/";

} // namespace app3D
} // namespace engine
