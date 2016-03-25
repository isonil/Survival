#include "Terrain.hpp"

#include "../../ext/PerlinNoise.hpp"
#include "../../util/WindGenerator.hpp"
#include "../../util/Exception.hpp"
#include "../managers/ResourcesManager.hpp"
#include "../managers/ShadersManager.hpp"
#include "../irrNodes/GrassPatch.hpp"
#include "../defs/TerrainDef.hpp"
#include "../Device.hpp"
#include "../IrrlichtConversions.hpp"

namespace engine
{
namespace app3D
{

Terrain::Terrain(const std::shared_ptr <TerrainDef> &terrainDef, const std::weak_ptr <Device> &device)
    : SceneNode{device},
      m_terrainDef{terrainDef}
{
    TRACK;

    if(!m_terrainDef)
        throw Exception{"Terrain def is nullptr."};

    m_windGenerator = std::make_shared <WindGenerator> (k_windStrength, k_windRegularity);

    createRender();
}

void Terrain::dropIrrObjects()
{
    TRACK;

    m_currentRender.terrainNode = nullptr;
    m_currentRender.terrainNode_helper = nullptr;
    m_currentRender.flatTerrainNode = nullptr;
    m_currentRender.deferredRendering = false;
    m_currentRender.grassPatches.clear();
}

void Terrain::reloadIrrObjects()
{
    TRACK;

    createRender();
}

bool Terrain::wantsEverUpdate() const
{
    return false;
}

bool Terrain::isFlat() const
{
    E_DASSERT(m_terrainDef, "Terrain def is nullptr.");

    return m_terrainDef->isFlat();
}

std::vector <float> Terrain::getHeightGrid() const
{
    E_DASSERT(m_terrainDef, "Terrain def is nullptr.");

    if(m_terrainDef->isFlat())
        throw Exception{"Tried to get height grid from flat terrain. "
                        "This exception was thrown to avoid potential performance loss "
                        "due to creating high poly physics engine body for flat terrain."};

    if(!m_currentRender.terrainNode)
        throw Exception{"Couldn't get height grid because terrain node is nullptr."};

    std::vector <float> vec;

    // old method (quite bad):

    /*
    auto scale = static_cast <int> (m_terrainDef->getScale());

    if(!scale)
        return vec;

    if(scale < 0)
        throw Exception{"Terrain scale is negative."};

    IntVec2 gridSize{scale, scale};

    m_currentRender.terrainNode->updateAbsolutePosition();

    const float terrainSizeEps{0.001f};

    // TODO: check if the grid shouldn't have size: scale + 1, scale + 1

    for(int i = 0; i < gridSize.y; ++i) {
        for(int j = 0; j < gridSize.x; ++j) {
            FloatVec2 at(m_pos.x + j, m_pos.z + i);

            if(i == 0)
                at.y += terrainSizeEps;

            if(j == 0)
                at.x += terrainSizeEps;

            if(i == gridSize.y - 1)
                at.y -= terrainSizeEps;

            if(j == gridSize.x - 1)
                at.x -= terrainSizeEps;

            float height{m_currentRender.terrainNode->getHeight(at.x, at.y)};

            vec.push_back(height);
        }
    }
    */

    // new method (more accurate):

    auto *mesh = m_currentRender.terrainNode->getMesh();

    if(!mesh->getMeshBufferCount())
        throw engine::Exception{"Mesh has no mesh buffers."};

    auto *meshBuffer = mesh->getMeshBuffer(0);

    E_DASSERT(meshBuffer->getVertexType() == irr::video::EVT_2TCOORDS, "Expected EVT_2TCOORDS vertex type.");

    std::vector <FloatVec3> vertices;

    for(irr::u32 i = 0; i < meshBuffer->getVertexCount(); ++i) {
        auto &vertex = static_cast <irr::video::S3DVertex2TCoords*> (meshBuffer->getVertices())[i];
        vertices.emplace_back(vertex.Pos.X, vertex.Pos.Y, vertex.Pos.Z);
    }

    std::sort(vertices.begin(), vertices.end(), [](const auto &lhs, const auto &rhs) {
        return std::tie(lhs.z, lhs.x) < std::tie(rhs.z, rhs.x);
    });

    float nodeYScale{m_currentRender.terrainNode->getScale().Y};

    for(const auto &elem : vertices) {
        vec.push_back(elem.y * nodeYScale);
    }

    return vec;
}

void Terrain::setPosition(const FloatVec3 &pos)
{
    if(pos != m_pos) {
        m_pos = pos;
        updateCurrentRenderPosition();
    }
}

const FloatVec3 &Terrain::getPosition() const
{
    return m_pos;
}

float Terrain::getHeight(const FloatVec2 &pos) const
{
    TRACK;

    E_DASSERT(m_terrainDef, "Terrain def is nullptr.");

    if(m_terrainDef->isFlat())
        return 0.f;

    if(!m_currentRender.terrainNode)
        throw Exception{"Terrain node is nullptr."};

    // TODO: add boundaries check (include +eps and -eps for border cases)

    return m_currentRender.terrainNode->getHeight(m_pos.x + pos.x, m_pos.z + pos.y);
}

TerrainDef &Terrain::getDef() const
{
    E_DASSERT(m_terrainDef, "Terrain def is nullptr.");

    return *m_terrainDef;
}

const std::shared_ptr <TerrainDef> &Terrain::getDefPtr() const
{
    E_DASSERT(m_terrainDef, "Terrain def is nullptr.");

    return m_terrainDef;
}

Terrain::~Terrain()
{
    TRACK;

    if(!deviceExpired())
        removeCurrentRender();
}

void Terrain::createRender()
{
    TRACK;

    removeCurrentRender();

    auto &device = getDevice_slow();
    auto &scene = *device.getIrrDevice().getSceneManager();
    auto &resourcesManager = device.getResourcesManager();

    E_DASSERT(m_terrainDef, "Terrain def is nullptr.");

    if(!m_terrainDef->isFlat()) {
        const auto &heightMapPath = m_terrainDef->getHeightMapPath();

        if(m_terrainDef->getMaxLOD() > 4)
            throw engine::Exception{"Max terrain LOD level is 4 (because irr::scene::ETPS_9 is used)."};

        m_currentRender.terrainNode = scene.addTerrainSceneNode(heightMapPath.c_str(),
            scene.getRootSceneNode(),
            -1,
            irr::core::vector3df{m_pos.x, m_pos.y, m_pos.z},
            irr::core::vector3df{0.f, 0.f, 0.f},
            irr::core::vector3df{1.f, 1.f, 1.f},
            irr::video::SColor{255, 255, 255, 255},
            m_terrainDef->getMaxLOD(),
            irr::scene::ETPS_9,
            m_terrainDef->getSmoothFactor(),
            true);

        if(!m_currentRender.terrainNode)
            throw Exception{"Could not add terrain scene node."};

        m_currentRender.terrainNode->setScale({m_terrainDef->getScale() / static_cast <float> (m_terrainDef->getHeightMap().getSize().Width),
                                               m_terrainDef->getScale() / 1024.f,
                                               m_terrainDef->getScale() / static_cast <float> (m_terrainDef->getHeightMap().getSize().Height)});

        m_currentRender.terrainNode->setMaterialTexture(0, &m_terrainDef->getTexture1());
        m_currentRender.terrainNode->setMaterialTexture(1, &m_terrainDef->getTexture2());
        m_currentRender.terrainNode->setMaterialTexture(2, &m_terrainDef->getTexture3());
        m_currentRender.terrainNode->setMaterialTexture(3, &m_terrainDef->getSlopeTexture());
        m_currentRender.terrainNode->setMaterialTexture(4, &m_terrainDef->getSplatMap());
        m_currentRender.terrainNode->setMaterialTexture(5, &m_terrainDef->getNormalMap());
        m_currentRender.terrainNode->setMaterialTexture(6, &resourcesManager.loadIrrTexture(k_causticsTexturePath, true));
        m_currentRender.terrainNode->scaleTexture(1.f, 30.f); // old value: 20.f
        m_currentRender.terrainNode->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, true);
        m_currentRender.terrainNode->setCameraMovementDelta(0.1f);
        m_currentRender.terrainNode->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, false);

        for(irr::u32 i = 0; i < m_currentRender.terrainNode->getMaterialCount(); ++i) {
            auto &material = m_currentRender.terrainNode->getMaterial(i);

            for(irr::u32 j = 0; j < irr::video::MATERIAL_MAX_TEXTURES; ++j) {
                material.TextureLayer[j].AnisotropicFilter = k_anisotropicFilterLevel;
                material.TextureLayer[j].BilinearFilter = true;
                material.TextureLayer[j].TrilinearFilter = false;
            }
        }

        m_currentRender.terrainNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
        m_currentRender.terrainNode->setMaterialFlag(irr::video::EMF_FOG_ENABLE, false);

        if(!Math::fuzzyCompare(m_terrainDef->getSlopeDistortion(), 0.f)) {
            // second value: higher - more pointy, third: height
            PerlinNoise noise{0.5f, 0.08f, m_terrainDef->getSlopeDistortion(), 3, 34573}; // 34573 is arbitrarly chosen seed

            auto *mesh = m_currentRender.terrainNode->getMesh();

            if(mesh->getMeshBufferCount()) {
                auto *meshBuffer = mesh->getMeshBuffer(0);

                E_DASSERT(meshBuffer->getVertexType() == irr::video::EVT_2TCOORDS, "Expected EVT_2TCOORDS vertex type.");

                const auto &normalMap = m_terrainDef->getNormalMapImage();

                for(irr::u32 i = 0; i < meshBuffer->getVertexCount(); ++i) {
                    auto &vertex = static_cast <irr::video::S3DVertex2TCoords*> (meshBuffer->getVertices())[i];

                    auto normalMapXPos = static_cast <int> (normalMap.getDimension().Width - vertex.Pos.X);

                    if(normalMapXPos >= static_cast <int> (normalMap.getDimension().Width))
                        --normalMapXPos;

                    auto normalMapZPos = static_cast <int> (vertex.Pos.Z);

                    E_DASSERT(normalMapXPos >= 0 && normalMapXPos < static_cast <int> (normalMap.getDimension().Width) &&
                              normalMapZPos >= 0 && normalMapZPos < static_cast <int> (normalMap.getDimension().Height), "Pixel coords out of bounds.");

                    auto normal = static_cast <int> (normalMap.getPixel(normalMapXPos, normalMapZPos).getBlue());

                    if(normal > k_slopeDistortionNormalMapBlueColorTransitionRange.to)
                        continue;

                    float factor{1.f};

                    if(normal > k_slopeDistortionNormalMapBlueColorTransitionRange.from)
                        factor = (k_slopeDistortionNormalMapBlueColorTransitionRange.to - normal) / static_cast <float> (k_slopeDistortionNormalMapBlueColorTransitionRange.getLength());

                    vertex.Pos.Y += std::fabs(noise.GetHeight(vertex.Pos.X, vertex.Pos.Z)) * factor;
                }
            }
        }

        // these LOD distance values seem to work best,
        // it's important that the first distance is at least 50, so that
        // terrain will not pop up in front of players eyes
        for(int i = 0; i < m_terrainDef->getMaxLOD(); ++i) {
            m_currentRender.terrainNode->overrideLODDistance(i, 50.f + i * 5.f);
        }

        // HACK: Irrlicht has a bug where ITerrainSceneNode's getAbsolutePosition() always returns 0,0,0
        // so we need a helper node which works fine
        m_currentRender.terrainNode_helper = scene.addEmptySceneNode(scene.getRootSceneNode());

        if(!m_currentRender.terrainNode_helper)
            throw Exception{"Could not add terrain helper node."};

        if(m_terrainDef->usesGrass()) {
            auto xCount = static_cast <int> (m_terrainDef->getScale() / irrNodes::GrassPatch::k_grassPatchSize) + 1;
            auto yCount = static_cast <int> (m_terrainDef->getScale() / irrNodes::GrassPatch::k_grassPatchSize) + 1;

            for(int i = 0; i < xCount; ++i) {
                for(int j = 0; j < yCount; ++j) {
                    m_currentRender.grassPatches.push_back(new irrNodes::GrassPatch{
                        scene, *m_currentRender.terrainNode, *m_currentRender.terrainNode_helper,
                        {(i + 0.5f) * irrNodes::GrassPatch::k_grassPatchSize, (j + 0.5f) * irrNodes::GrassPatch::k_grassPatchSize}, {m_terrainDef->getScale(), m_terrainDef->getScale()},
                        m_terrainDef->getNormalMapImage(), m_terrainDef->getSplatMapImage(),
                        m_terrainDef->getGrassColor(), k_grassTexturesInTexture, m_windGenerator
                    });

                    auto *grass = m_currentRender.grassPatches.back();

                    grass->setMaterialType(irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF);
                    grass->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, false);
                    grass->setMaterialFlag(irr::video::EMF_FOG_ENABLE, true);
                    grass->setMaterialFlag(irr::video::EMF_LIGHTING, true);
                    grass->setMaterialTexture(0, &resourcesManager.loadIrrTexture(k_grassTexturePath, true));
                    grass->drop();
                }
            }
        }
    }
    else {
        auto &mesh = device.getResourcesManager().getSimplePlaneMesh();

        m_currentRender.flatTerrainNode = scene.addMeshSceneNode(&mesh);

        if(!m_currentRender.flatTerrainNode)
            throw Exception{"Could not add flat terrain node."};

        auto scale = m_terrainDef->getScale();
        m_currentRender.flatTerrainNode->setScale({scale, scale, scale});

        auto &material = m_currentRender.flatTerrainNode->getMaterial(0);

        material.TextureLayer[0].Texture = &m_terrainDef->getTexture1();
        material.TextureLayer[0].AnisotropicFilter = k_anisotropicFilterLevel;
        material.TextureLayer[0].BilinearFilter = true;
        material.TextureLayer[0].TrilinearFilter = false;

        material.TextureLayer[1].Texture = &resourcesManager.loadIrrTexture(k_causticsTexturePath, true);

        m_currentRender.flatTerrainNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
        m_currentRender.flatTerrainNode->setMaterialFlag(irr::video::EMF_FOG_ENABLE, false);
    }

    updateCurrentRenderPosition();
    updateCurrentRenderMaterial();
}

void Terrain::removeCurrentRender()
{
    TRACK;

    if(m_currentRender.terrainNode) {
        m_currentRender.terrainNode->remove();
        m_currentRender.terrainNode = nullptr;
    }

    if(m_currentRender.flatTerrainNode) {
        m_currentRender.flatTerrainNode->remove();
        m_currentRender.flatTerrainNode = nullptr;
    }

    m_currentRender.deferredRendering = false;
}

void Terrain::updateCurrentRenderPosition()
{
    TRACK;

    if(m_currentRender.terrainNode) {
        const auto &irrPos = IrrlichtConversions::toVector(m_pos);
        m_currentRender.terrainNode->setPosition(irrPos);
        m_currentRender.terrainNode_helper->setPosition(irrPos);
    }

    if(m_currentRender.flatTerrainNode) {
        E_DASSERT(m_terrainDef, "Terrain def is nullptr.");

        auto scale = m_terrainDef->getScale();
        // we have to move flat terrain node by an offset, because Irrlicht terrain node
        // sets node position to one of its corners position rather than node center
        const auto &irrPos = IrrlichtConversions::toVector(m_pos.moved(scale / 2.f, 0.f, scale / 2.f));
        m_currentRender.flatTerrainNode->setPosition(irrPos);
    }
}

void Terrain::updateCurrentRenderMaterial()
{
    TRACK;

    auto &device = getDevice_slow();

    bool shouldUseDeferred{device.isUsingDeferredRendering()};

    if(shouldUseDeferred) {
        m_currentRender.deferredRendering = true;

        if(m_currentRender.terrainNode)
            m_currentRender.terrainNode->setMaterialType(device.getShadersManager().getTerrainDeferredShaderMaterialType());

        if(m_currentRender.flatTerrainNode)
            m_currentRender.flatTerrainNode->setMaterialType(device.getShadersManager().getFlatTerrainDeferredShaderMaterialType());
    }
    else {
        m_currentRender.deferredRendering = false;

        if(m_currentRender.terrainNode)
            m_currentRender.terrainNode->setMaterialType(device.getShadersManager().getTerrainShaderMaterialType());

        if(m_currentRender.flatTerrainNode)
            m_currentRender.flatTerrainNode->setMaterialType(device.getShadersManager().getFlatTerrainShaderMaterialType());
    }
}

const int Terrain::k_anisotropicFilterLevel{8};
const std::string Terrain::k_causticsTexturePath = "caustics.png";
const float Terrain::k_windStrength{4.f};
const float Terrain::k_windRegularity{5.f};
const std::string Terrain::k_grassTexturePath = "grassMesh.png";
const IntVec2 Terrain::k_grassTexturesInTexture{3, 1};
const IntRange Terrain::k_slopeDistortionNormalMapBlueColorTransitionRange{175, 225};

} // namespace app3D
} // namespace engine
