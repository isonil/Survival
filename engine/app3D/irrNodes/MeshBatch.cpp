#include "MeshBatch.hpp"

#include "../../util/Exception.hpp"
#include "../../util/LogManager.hpp"
#include "../../util/Trace.hpp"

#include <numeric>

namespace engine
{
namespace app3D
{
namespace irrNodes
{

MeshBatch::MeshBatch(irr::scene::ISceneManager &sceneManager, irr::video::ITexture &textureAtlas, irr::video::E_MATERIAL_TYPE defaultMaterialType, irr::video::E_MATERIAL_TYPE defaultDeferredRenderingMaterialType)
    :   ISceneNode(sceneManager.getRootSceneNode(), &sceneManager),
        m_deferredRendering{},
        m_defaultMaterialType{defaultMaterialType},
        m_defaultDeferredRenderingMaterialType{defaultDeferredRenderingMaterialType},
        m_recalculateMeshBuffer{},
        m_reportedBadTexCoords{}
{
    TRACK;

    m_meshes.reserve(500);

    m_material.TextureLayer[0].Texture = &textureAtlas;
    m_material.TextureLayer[0].AnisotropicFilter = 8;
    m_material.TextureLayer[0].BilinearFilter = true;
    m_material.TextureLayer[0].TrilinearFilter = false;
    m_material.BackfaceCulling = true;

    m_meshBuffer.setHardwareMappingHint(irr::scene::EHM_STATIC);

    updateMaterial(false); // TODO: bool depending on current deferred status (pass to constructor?)
}

void MeshBatch::OnRegisterSceneNode()
{
    TRACK;

    SceneManager->registerNodeForRendering(this, irr::scene::ESNRP_SOLID);
}

void MeshBatch::render()
{
    TRACK;

    updateMeshes();

    auto &driver = *SceneManager->getVideoDriver();

    driver.setTransform(irr::video::ETS_WORLD, irr::core::matrix4{});
    driver.setMaterial(m_material);
    driver.drawMeshBuffer(&m_meshBuffer);
}

irr::video::SMaterial &MeshBatch::getMaterial(irr::u32)
{
    return m_material;
}

irr::u32 MeshBatch::getMaterialCount() const
{
    return 1;
}

const irr::core::aabbox3df &MeshBatch::getBoundingBox() const
{
    // TODO: find a better way to define infinite bounding box?
    static const irr::core::aabbox3df box{{-10000.f, -10000.f, -10000.f}, {10000.f, 10000.f, 10000.f}};
    return box;
}

void MeshBatch::updateMaterial(bool useDeferredRendering)
{
    TRACK;

    if(useDeferredRendering) {
        m_deferredRendering = true;
        m_material.MaterialType = m_defaultDeferredRenderingMaterialType;
        m_material.FogEnable = false;
    }
    else {
        m_deferredRendering = false;
        m_material.MaterialType = m_defaultMaterialType;
        m_material.FogEnable = true;
    }

    bool wantsLighting{true};

    /*
    // TODO: remove?
    // HACK: assuming transparent meshes don't want lighting
    if(useDeferredRendering ||
       m_material.MaterialType == irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL ||
       m_material.MaterialType == irr::video::EMT_TRANSPARENT_ADD_COLOR ||
       m_material.MaterialType == irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF)
        wantsLighting = false;
    */

    // HACK: forcing some specular for solid meshes, (maybe not really a hack? maybe it's good?)
    if(m_material.MaterialType == irr::video::EMT_SOLID) {
        m_material.Shininess = 20.f;
        m_material.SpecularColor.set(255, 128, 128, 128);
    }

    m_material.Lighting = wantsLighting;
}

std::shared_ptr <int> MeshBatch::addMesh(irr::scene::IMesh &mesh, const std::vector <FloatRect> &textureAtlasRects, bool forceAllUpNormals)
{
    TRACK;

    if(textureAtlasRects.size() != mesh.getMeshBufferCount())
        throw Exception{"Mesh texture atlas rect count does not equal mesh buffer count."};

    int meshVertexCount{};

    for(irr::u32 i = 0; i < mesh.getMeshBufferCount(); ++i) {
        meshVertexCount += mesh.getMeshBuffer(i)->getVertexCount();
    }

   if(getVertexCount() + meshVertexCount > k_maxVertices)
       throw Exception{"Mesh batch has reached maximum vertex count."};

    m_meshes.resize(m_meshes.size() + 1);

    auto index = static_cast <int> (m_meshes.size() - 1);

    m_meshes.back().index = std::make_shared <int> (index);
    m_meshes.back().verticesStartIndex = static_cast <int> (m_meshBuffer.Vertices.size());

    int totalVertexCount = 0; // needed if there is more than one mesh buffer per mesh

    bool badTexCoords{};
    FloatVec2 badTexCoordsValue;
    int badTexCoordsVertexIndex{-1};

    for(irr::u32 i = 0; i < mesh.getMeshBufferCount(); ++i) {
        E_DASSERT(mesh.getMeshBuffer(i)->getVertexType() == irr::video::EVT_STANDARD, "Expected EVT_STANDARD vertex type.");

        for(irr::u32 j = 0; j < mesh.getMeshBuffer(i)->getVertexCount(); ++j) {
            irr::video::S3DVertex vertex{static_cast <irr::video::S3DVertex*> (mesh.getMeshBuffer(i)->getVertices())[j]};

            float tx{textureAtlasRects[i].pos.x};
            float ty{textureAtlasRects[i].pos.y};
            float tw{textureAtlasRects[i].size.x};
            float th{textureAtlasRects[i].size.y};

            // clamp all > 1.f and near 1.f values to 1.f

            if(vertex.TCoords.X > 1.f && Math::fuzzyCompare(vertex.TCoords.X, 1.f, 0.01f))
                vertex.TCoords.X = 1.f;

            if(vertex.TCoords.Y > 1.f && Math::fuzzyCompare(vertex.TCoords.Y, 1.f, 0.01f))
                vertex.TCoords.Y = 1.f;

            // clamp all < 0.f and near 0.f values to 0.f

            if(vertex.TCoords.X < 0.f && Math::fuzzyCompare(vertex.TCoords.X, 0.f, 0.01f))
                vertex.TCoords.X = 0.f;

            if(vertex.TCoords.Y < 0.f && Math::fuzzyCompare(vertex.TCoords.Y, 0.f, 0.01f))
                vertex.TCoords.Y = 0.f;

            // check if bad

            if(vertex.TCoords.X < 0.f || vertex.TCoords.X > 1.f ||
               vertex.TCoords.Y < 0.f || vertex.TCoords.Y > 1.f) {
                if(!badTexCoords) {
                    badTexCoords = true;
                    badTexCoordsValue.set(vertex.TCoords.X, vertex.TCoords.Y);
                    badTexCoordsVertexIndex = totalVertexCount + static_cast <int> (j);
                }

                // now we have to somehow 'repair' tex coords to keep them in range <0, 1>;
                // clamp or fmod

                //vertex.TCoords.X = Math::clamp01(vertex.TCoords.X);
                //vertex.TCoords.Y = Math::clamp01(vertex.TCoords.Y);

                vertex.TCoords.X = fmod(vertex.TCoords.X, 1.f);
                vertex.TCoords.Y = fmod(vertex.TCoords.Y, 1.f);

                if(vertex.TCoords.X < 0.f)
                    vertex.TCoords.X += 1.f;

                if(vertex.TCoords.Y < 0.f)
                    vertex.TCoords.Y += 1.f;
            }

            vertex.TCoords.X = vertex.TCoords.X * tw + tx;
            vertex.TCoords.Y = vertex.TCoords.Y * th + ty;

            if(forceAllUpNormals)
                vertex.Normal = {0.f, 1.f, 0.f};

            m_meshes.back().vertices.push_back(vertex);

            if(!m_recalculateMeshBuffer) {
                irr::video::S3DVertex vertex2{vertex};
                transformVertex(vertex2.Pos, vertex2.Normal, m_meshes.back());
                m_meshBuffer.Vertices.push_back(vertex2);
            }
        }

        for(irr::u32 j = 0; j < mesh.getMeshBuffer(i)->getIndexCount(); ++j) {
            auto index = static_cast <irr::u16> (totalVertexCount) + mesh.getMeshBuffer(i)->getIndices()[j];
            m_meshes.back().indices.push_back(index);

            if(!m_recalculateMeshBuffer)
                m_meshBuffer.Indices.push_back(index + m_meshes.back().verticesStartIndex);
        }

        totalVertexCount += mesh.getMeshBuffer(i)->getVertexCount();
    }

    if(badTexCoords && !m_reportedBadTexCoords) {
        E_WARNING("Mesh has incorrect tex coords to be used by mesh batch. Artifacts may appear. "
                  "First occurence: (vertex %d, tex coords: %.3f %.3f). "
                  "Bad tex coords warnings will be suppressed from now on for this mesh batch.",
                  badTexCoordsVertexIndex, badTexCoordsValue.x, badTexCoordsValue.y);

        m_reportedBadTexCoords = true;
    }

    m_meshes.back().verticesEndIndex = static_cast <int> (m_meshBuffer.Vertices.size());

    m_meshBuffer.setDirty();

    return m_meshes.back().index;
}

void MeshBatch::setMeshPosition(int index, const irr::core::vector3df &pos)
{
    TRACK;

    if(index < 0 || static_cast <size_t> (index) >= m_meshes.size())
        return;

    if(m_meshes[index].pos != pos) {
        m_meshes[index].pos = pos;

        if(!m_recalculateMeshBuffer) {
            E_DASSERT(static_cast <int> (m_meshes[index].vertices.size()) == m_meshes[index].verticesEndIndex - m_meshes[index].verticesStartIndex,
                      "Invalid mesh vertices count.");

            E_DASSERT(m_meshes[index].verticesStartIndex >= 0 && static_cast <size_t> (m_meshes[index].verticesEndIndex) <= m_meshBuffer.Vertices.size(),
                      "Mesh vertex index out of bounds while accessing batch mesh buffer.");

            for(int i = m_meshes[index].verticesStartIndex, j = 0; i < m_meshes[index].verticesEndIndex; ++i, ++j) {
                irr::core::vector3df verPos{m_meshes[index].vertices[j].Pos};
                irr::core::vector3df verNormal{m_meshes[index].vertices[j].Normal};
                transformVertex(verPos, verNormal, m_meshes[index]);
                m_meshBuffer.Vertices[i].Pos = verPos;
                m_meshBuffer.Vertices[i].Normal = verNormal;
            }
        }

        m_meshBuffer.setDirty(irr::scene::EBT_VERTEX);
    }
}

void MeshBatch::setMeshRotation(int index, const irr::core::vector3df &rot)
{
    TRACK;

    if(index < 0 || static_cast <size_t> (index) >= m_meshes.size())
        return;

    if(m_meshes[index].rot != rot) {
        m_meshes[index].rot = rot;

        irr::core::matrix4 mp;
        mp.setRotationDegrees(rot);

        m_meshes[index].rotationMatrix = mp;

        if(!m_recalculateMeshBuffer) {
            E_DASSERT(static_cast <int> (m_meshes[index].vertices.size()) == m_meshes[index].verticesEndIndex - m_meshes[index].verticesStartIndex,
                      "Invalid mesh vertices count.");

            E_DASSERT(m_meshes[index].verticesStartIndex >= 0 && static_cast <size_t> (m_meshes[index].verticesEndIndex) <= m_meshBuffer.Vertices.size(),
                      "Mesh vertex index out of bounds while accessing batch mesh buffer.");

            for(int i = m_meshes[index].verticesStartIndex, j = 0; i < m_meshes[index].verticesEndIndex; ++i, ++j) {
                irr::core::vector3df verPos{m_meshes[index].vertices[j].Pos};
                irr::core::vector3df verNormal{m_meshes[index].vertices[j].Normal};
                transformVertex(verPos, verNormal, m_meshes[index]);
                m_meshBuffer.Vertices[i].Pos = verPos;
                m_meshBuffer.Vertices[i].Normal = verNormal;
            }
        }

        m_meshBuffer.setDirty(irr::scene::EBT_VERTEX);
    }
}

void MeshBatch::setMeshScale(int index, const irr::core::vector3df &scale)
{
    TRACK;

    if(index < 0 || static_cast <size_t> (index) >= m_meshes.size())
        return;

    if(m_meshes[index].scale != scale) {
        m_meshes[index].scale = scale;

        if(!m_recalculateMeshBuffer) {
            for(int i = m_meshes[index].verticesStartIndex, j = 0; i < m_meshes[index].verticesEndIndex; ++i, ++j) {
                irr::core::vector3df verPos{m_meshes[index].vertices[j].Pos};
                irr::core::vector3df verNormal{m_meshes[index].vertices[j].Normal};
                transformVertex(verPos, verNormal, m_meshes[index]);
                m_meshBuffer.Vertices[i].Pos = verPos;
                m_meshBuffer.Vertices[i].Normal = verNormal;
            }
        }

        m_meshBuffer.setDirty(irr::scene::EBT_VERTEX);
    }
}

void MeshBatch::removeMesh(int index)
{
    TRACK;

    if(index < 0 || static_cast <size_t> (index) >= m_meshes.size())
        return;

    size_t lastIndex{m_meshes.size() - 1};
    std::swap(m_meshes[index], m_meshes[lastIndex]);

    E_DASSERT(m_meshes[index].index, "Index is nullptr.");
    E_DASSERT(m_meshes[lastIndex].index, "Index is nullptr.");

    *m_meshes[lastIndex].index = -1;
    m_meshes.pop_back();

    if(static_cast <size_t> (index) != lastIndex)
        *m_meshes[index].index = index;

    m_recalculateMeshBuffer = true;

    m_meshBuffer.setDirty();
}

int MeshBatch::getMeshCount() const
{
    return m_meshes.size();
}

int MeshBatch::getVertexCount() const
{
    TRACK;

    if(m_recalculateMeshBuffer) {
        return std::accumulate(m_meshes.begin(), m_meshes.end(), 0, [](int sum, const auto &m) {
            return sum + m.vertices.size();
        });
    }
    else
        return m_meshBuffer.Vertices.size();
}

irr::video::E_MATERIAL_TYPE MeshBatch::getDefaultMaterialType() const
{
    return m_defaultMaterialType;
}

irr::video::E_MATERIAL_TYPE MeshBatch::getDefaultDeferredRenderingMaterialType() const
{
    return m_defaultDeferredRenderingMaterialType;
}

MeshBatch::Mesh::Mesh()
    : scale{1.f, 1.f, 1.f},
      verticesStartIndex{},
      verticesEndIndex{}
{
    rotationMatrix.setRotationDegrees({0.f, 0.f, 0.f});
}

void MeshBatch::transformVertex(irr::core::vector3df &ver, irr::core::vector3df &normal, const Mesh &mesh)
{
    ver *= mesh.scale;
    mesh.rotationMatrix.rotateVect(ver);
    ver += mesh.pos;

    mesh.rotationMatrix.rotateVect(normal);
}

void MeshBatch::updateMeshes()
{
    TRACK;

    if(m_recalculateMeshBuffer) {
        // in order to avoid reallocation, we overwrite mesh buffer vertices and indices

        size_t currentVertexIndex{};
        size_t currentIndexIndex{};

        for(size_t i = 0; i < m_meshes.size(); ++i) {
            m_meshes[i].verticesStartIndex = currentVertexIndex;

            for(size_t j = 0; j < m_meshes[i].vertices.size(); ++j) {
                irr::video::S3DVertex newVertex{m_meshes[i].vertices[j]};

                transformVertex(newVertex.Pos, newVertex.Normal, m_meshes[i]);

                if(currentVertexIndex < m_meshBuffer.Vertices.size())
                    m_meshBuffer.Vertices[currentVertexIndex] = newVertex;
                else
                    m_meshBuffer.Vertices.push_back(newVertex);

                ++currentVertexIndex;
            }

            m_meshes[i].verticesEndIndex = currentVertexIndex;

            for(size_t j = 0; j < m_meshes[i].indices.size(); ++j) {
                auto index = static_cast <irr::u16> (m_meshes[i].indices[j] + (m_meshes[i].verticesStartIndex));

                if(currentIndexIndex < m_meshBuffer.Indices.size())
                    m_meshBuffer.Indices[currentIndexIndex] = index;
                else
                    m_meshBuffer.Indices.push_back(index);

                ++currentIndexIndex;
            }
        }

        if(currentVertexIndex < m_meshBuffer.Vertices.size())
            m_meshBuffer.Vertices.erase(currentVertexIndex, m_meshBuffer.Vertices.size() - currentVertexIndex);

        if(currentIndexIndex < m_meshBuffer.Indices.size())
            m_meshBuffer.Indices.erase(currentIndexIndex, m_meshBuffer.Indices.size() - currentIndexIndex);

        m_recalculateMeshBuffer = false;
        m_meshBuffer.setDirty();
    }
}

int MeshBatch::k_maxVertices{50000};

} // namespace irrNodes
} // namespace app3D
} // namespace engine
