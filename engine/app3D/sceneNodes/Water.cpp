#include "Water.hpp"

#include "../Device.hpp"
#include "../managers/ResourcesManager.hpp"
#include "../managers/ShadersManager.hpp"
#include "../defs/TerrainDef.hpp"
#include "../IrrlichtConversions.hpp"

namespace engine
{
namespace app3D
{

Water::Water(const std::shared_ptr <TerrainDef> &terrainDef, const std::weak_ptr <Device> &device)
    : SceneNode{device},
      m_currentRender{},
      m_scale{1.f, 1.f, 1.f},
      m_terrainDef{terrainDef}
{
    if(!m_terrainDef)
        throw Exception{"Terrain def is nullptr."};

    createRender();
}

void Water::dropIrrObjects()
{
    m_currentRender.waterNode = nullptr;
    m_currentRender.deferredRendering = false;
}

void Water::reloadIrrObjects()
{
    createRender();
}

bool Water::wantsEverUpdate() const
{
    return false;
}

void Water::setPosition(const FloatVec3 &pos)
{
    if(pos != m_pos) {
        m_pos = pos;
        updateCurrentRenderPosition();
    }
}

const FloatVec3 &Water::getPosition() const
{
    return m_pos;
}

void Water::setScale(const FloatVec3 &scale)
{
    if(scale != m_scale) {
        m_scale = scale;
        updateCurrentRenderScale();
    }
}

const FloatVec3 &Water::getScale() const
{
    return m_scale;
}

Water::~Water()
{
    TRACK;

    if(!deviceExpired())
        removeCurrentRender();
}

void Water::createRender()
{
    TRACK;

    removeCurrentRender();

    auto &device = getDevice_slow();
    auto &resourcesManager = device.getResourcesManager();
    auto &mesh = resourcesManager.getSimplePlaneMesh();

    m_currentRender.waterNode = device.getIrrDevice().getSceneManager()->addMeshSceneNode(&mesh);

    if(!m_currentRender.waterNode)
        throw Exception{"Could not add water scene node."};

    auto &material = m_currentRender.waterNode->getMaterial(0);

    material.MaterialType = device.getShadersManager().getWaterShaderMaterialType();
    material.TextureLayer[0].Texture = &resourcesManager.loadIrrTexture(k_waterTexturePath, true);

    E_DASSERT(m_terrainDef, "Terrain def is nullptr.");

    if(!m_terrainDef->isFlat())
        material.TextureLayer[1].Texture = &m_terrainDef->getHeightMap();
    else
        material.TextureLayer[1].Texture = &resourcesManager.getBlackTexture();

    material.TextureLayer[2].Texture = &resourcesManager.loadIrrTexture(k_foamTexturePath, true);

    m_currentRender.waterNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    m_currentRender.waterNode->setMaterialFlag(irr::video::EMF_FOG_ENABLE, false);
    m_currentRender.waterNode->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, false);

    updateCurrentRenderPosition();
    updateCurrentRenderScale();
}

void Water::removeCurrentRender()
{
    TRACK;

    if(m_currentRender.waterNode) {
        m_currentRender.waterNode->remove();
        m_currentRender.waterNode = nullptr;
    }

    m_currentRender.deferredRendering = false;
}

void Water::updateCurrentRenderPosition()
{
    TRACK;

    if(m_currentRender.waterNode) {
        const auto &irrPos = IrrlichtConversions::toVector(m_pos);
        m_currentRender.waterNode->setPosition(irrPos);
    }
}

void Water::updateCurrentRenderScale()
{
    TRACK;

    if(m_currentRender.waterNode) {
        const auto &irrScale = IrrlichtConversions::toVector(m_scale);
        m_currentRender.waterNode->setScale(irrScale);
    }
}

const std::string Water::k_waterTexturePath = "water.png";
const std::string Water::k_foamTexturePath = "foam.png";

} // namespace app3D
} // namespace engine
