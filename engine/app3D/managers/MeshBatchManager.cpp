#include "MeshBatchManager.hpp"

#include "../../util/Exception.hpp"
#include "../../util/LogManager.hpp"
#include "../irrNodes/MeshBatch.hpp"
#include "../Device.hpp"
#include "../IrrlichtConversions.hpp"
#include "ResourcesManager.hpp"

#include <utility>

namespace engine
{
namespace app3D
{

MeshBatchManager::MeshBatchManager(Device &device)
    : m_device{device}
{
    m_batchedMeshes.reserve(500);
}

void MeshBatchManager::registerMesh(irr::scene::IMesh &mesh, const std::string &batchTag)
{
    TRACK;

    const auto &key = std::make_pair(&mesh, batchTag);
    const auto &it = m_registeredMeshes.find(key);

    if(it == m_registeredMeshes.end())
        m_registeredMeshes.emplace(key, RegisteredMesh{});
}

std::shared_ptr <int> MeshBatchManager::addMesh(irr::scene::IMesh &mesh, const std::string &batchTag, bool forceAllUpNormals)
{
    TRACK;

    auto &m = m_registeredMeshes[std::make_pair(&mesh, batchTag)];

    // if this mesh hasn't been added to any batch yet, then recreate all batches
    if(m.batchIndex < 0)
        createBatches();

    E_DASSERT(m.batchIndex >= 0 && static_cast <size_t> (m.batchIndex) < m_batches.size(), "Batch index out of bounds.");
    E_DASSERT(!m_batches[m.batchIndex].empty(), "There must be at least one batch available.");
    E_DASSERT(m_batches[m.batchIndex][0], "First batch is nullptr.");

    int batchSecondLevelIndex{-1};

    for(size_t i = 0; i < m_batches[m.batchIndex].size(); ++i) {
        E_DASSERT(m_batches[m.batchIndex][i], "Batch is nullptr.");

        if(m_batches[m.batchIndex][i]->getVertexCount() < k_maxVerticesPerBatch) {
            batchSecondLevelIndex = static_cast <int> (i);
            break;
        }
    }

    if(batchSecondLevelIndex < 0) {
        auto &batch = *m_batches[m.batchIndex][0];
        auto *tex = batch.getMaterial(0).TextureLayer[0].Texture;
        auto matType1 = batch.getDefaultMaterialType();
        auto matType2 = batch.getDefaultDeferredRenderingMaterialType();

        E_DASSERT(tex, "Mesh batch texture is nullptr.");

        m_batches[m.batchIndex].push_back(new irrNodes::MeshBatch{*m_device.getIrrDevice().getSceneManager(),
                                                                  *tex,
                                                                  matType1,
                                                                  matType2});

        // it's added to the Irrlicht scene manager, so we can drop it
        m_batches[m.batchIndex].back()->drop();
        batchSecondLevelIndex = static_cast <int> (m_batches[m.batchIndex].size() - 1);
    }

    const auto &batchedMeshIndex = m_batches[m.batchIndex][batchSecondLevelIndex]->addMesh(mesh, m.textureAtlasRects, forceAllUpNormals);

    m_batchedMeshes.emplace_back();

    auto &added = m_batchedMeshes.back();

    added.mesh = &mesh;
    added.batchTag = batchTag;
    added.batchIndex = m.batchIndex;
    added.batchSecondLevelIndex = batchSecondLevelIndex;
    added.batchedMeshIndex = batchedMeshIndex;
    added.index = std::make_shared <int> (m_batchedMeshes.size() - 1);
    added.forceAllUpNormals = forceAllUpNormals;

    return added.index;
}

void MeshBatchManager::setMeshPosition(int index, const FloatVec3 &pos)
{
    TRACK;

    if(index < 0 || static_cast <size_t> (index) >= m_batchedMeshes.size())
        return;

    auto &batchedMesh = m_batchedMeshes[index];

    E_DASSERT(batchedMesh.batchIndex >= 0 && static_cast <size_t> (batchedMesh.batchIndex) < m_batches.size(),
              "Batched mesh batch index out of bounds.");

    E_DASSERT(batchedMesh.batchedMeshIndex, "Batched mesh index is nullptr.");

    E_DASSERT(batchedMesh.batchSecondLevelIndex >= 0 && static_cast <size_t> (batchedMesh.batchSecondLevelIndex) < m_batches[batchedMesh.batchIndex].size(),
              "Batched mesh batch second level index out of bounds.");

    E_DASSERT(m_batches[batchedMesh.batchIndex][batchedMesh.batchSecondLevelIndex],
              "Batch is nullptr.");

    const auto &irrPos = IrrlichtConversions::toVector(pos);
    m_batches[batchedMesh.batchIndex][batchedMesh.batchSecondLevelIndex]->setMeshPosition(*batchedMesh.batchedMeshIndex, irrPos);
}

void MeshBatchManager::setMeshRotation(int index, const FloatVec3 &rot)
{
    TRACK;

    if(index < 0 || static_cast <size_t> (index) >= m_batchedMeshes.size())
        return;

    auto &batchedMesh = m_batchedMeshes[index];

    E_DASSERT(batchedMesh.batchIndex >= 0 && static_cast <size_t> (batchedMesh.batchIndex) < m_batches.size(),
              "Batched mesh batch index out of bounds.");

    E_DASSERT(batchedMesh.batchedMeshIndex, "Batched mesh index is nullptr.");

    E_DASSERT(batchedMesh.batchSecondLevelIndex >= 0 && static_cast <size_t> (batchedMesh.batchSecondLevelIndex) < m_batches[batchedMesh.batchIndex].size(),
              "Batched mesh batch second level index out of bounds.");

    E_DASSERT(m_batches[batchedMesh.batchIndex][batchedMesh.batchSecondLevelIndex],
              "Batch is nullptr.");

    const auto &irrRot = IrrlichtConversions::toVector(rot);
    m_batches[batchedMesh.batchIndex][batchedMesh.batchSecondLevelIndex]->setMeshRotation(*batchedMesh.batchedMeshIndex, irrRot);
}

void MeshBatchManager::setMeshScale(int index, const FloatVec3 &scale)
{
    TRACK;

    if(index < 0 || static_cast <size_t> (index) >= m_batchedMeshes.size())
        return;

    auto &batchedMesh = m_batchedMeshes[index];

    E_DASSERT(batchedMesh.batchIndex >= 0 && static_cast <size_t> (batchedMesh.batchIndex) < m_batches.size(),
              "Batched mesh batch index out of bounds.");

    E_DASSERT(batchedMesh.batchedMeshIndex, "Batched mesh index is nullptr.");

    E_DASSERT(batchedMesh.batchSecondLevelIndex >= 0 && static_cast <size_t> (batchedMesh.batchSecondLevelIndex) < m_batches[batchedMesh.batchIndex].size(),
              "Batched mesh batch second level index out of bounds.");

    E_DASSERT(m_batches[batchedMesh.batchIndex][batchedMesh.batchSecondLevelIndex],
              "Batch is nullptr.");

    const auto &irrScale = IrrlichtConversions::toVector(scale);
    m_batches[batchedMesh.batchIndex][batchedMesh.batchSecondLevelIndex]->setMeshScale(*batchedMesh.batchedMeshIndex, irrScale);
}

void MeshBatchManager::removeMesh(int index)
{
    TRACK;

    if(index < 0 || static_cast <size_t> (index) >= m_batchedMeshes.size())
       return;

    auto &batchedMesh = m_batchedMeshes[index];

    E_DASSERT(batchedMesh.batchIndex >= 0 && static_cast <size_t> (batchedMesh.batchIndex) < m_batches.size(),
              "Batched mesh batch index out of bounds.");

    E_DASSERT(batchedMesh.batchedMeshIndex, "Batched mesh index is nullptr.");

    E_DASSERT(batchedMesh.batchSecondLevelIndex >= 0 && static_cast <size_t> (batchedMesh.batchSecondLevelIndex) < m_batches[batchedMesh.batchIndex].size(),
              "Batched mesh batch second level index out of bounds.");

    E_DASSERT(m_batches[batchedMesh.batchIndex][batchedMesh.batchSecondLevelIndex],
              "Batch is nullptr.");

    m_batches[batchedMesh.batchIndex][batchedMesh.batchSecondLevelIndex]->removeMesh(*batchedMesh.batchedMeshIndex);

    // batchedMesh variable invalidated

    size_t lastIndex{m_batchedMeshes.size() - 1};
    std::swap(m_batchedMeshes[index], m_batchedMeshes[lastIndex]);

    E_DASSERT(m_batchedMeshes[index].index, "Index pointer is nullptr.");
    E_DASSERT(m_batchedMeshes[lastIndex].index, "Last index index pointer is nullptr.");

    *m_batchedMeshes[lastIndex].index = -1;
    m_batchedMeshes.pop_back();

    if(static_cast <size_t> (index) != lastIndex)
        *m_batchedMeshes[index].index = index;
}

void MeshBatchManager::createBatches()
{
    TRACK;

    E_INFO("Creating mesh batches.");

    for(auto &elem : m_batchedMeshes) {
        E_DASSERT(elem.batchedMeshIndex, "Batched mesh index is nullptr.");
        *elem.batchedMeshIndex = -1;
    }

    for(auto &elem1 : m_batches) {
        for(auto &elem2 : elem1) {
            E_DASSERT(elem2, "Batch is nullptr.");
            elem2->remove();
        }
    }

    m_batches.clear();

    for(auto &elem : m_registeredMeshes) {
        elem.second.batchIndex = -1;
        elem.second.textureAtlasRects.clear();
    }

    if(!m_registeredMeshes.empty()) {
        std::set <std::string> batchTags;

        for(auto &elem : m_registeredMeshes) {
            batchTags.insert(elem.first.second);
        }

        for(auto &b : batchTags) {
            std::vector <irr::scene::IMesh *> meshes;

            for(auto &m : m_registeredMeshes) {
                if(m.first.second == b)
                    meshes.push_back(m.first.first);
            }

            if(!meshes.empty()) {
                if(!packTextures(meshes, 0, meshes.size() - 1, b)) {
                    if(b.empty())
                        throw Exception{"Textures did not fit in packed textures."};
                    else
                        throw Exception{"Textures did not fit in packed textures (batch tag \"" + b + "\")."};
                }
            }
        }

        E_INFO("Mesh batches count: %d.", static_cast <int> (m_batches.size()));
    }

    for(auto &elem : m_batchedMeshes) {
        E_DASSERT(elem.mesh, "Mesh is nullptr.");
        elem.batchedMeshIndex = addMesh(*elem.mesh, elem.batchTag, elem.forceAllUpNormals);
    }
}

bool MeshBatchManager::packTextures(const std::vector <irr::scene::IMesh *> &meshes, int from, int to, const std::string &batchTag)
{
    TRACK;

    E_DASSERT(to - from >= 0, "Empty range.");

    std::set <irr::video::ITexture *> meshTexturesSet;
    bool wantsTransparency{};

    for(int i = from; i <= to; ++i) {
        E_DASSERT(meshes[i], "Mesh is nullptr.");

        for(irr::u32 j = 0; j < meshes[i]->getMeshBufferCount(); ++j) {
            auto &mat = meshes[i]->getMeshBuffer(j)->getMaterial();
            auto *meshTex = mat.TextureLayer[0].Texture;

            if(meshTex) {
                meshTexturesSet.insert(meshTex);

                if(mat.MaterialType == irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL ||
                   mat.MaterialType == irr::video::EMT_TRANSPARENT_ADD_COLOR ||
                   mat.MaterialType == irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF)
                    wantsTransparency = true;
            }
        }
    }

    std::vector <irr::video::ITexture *> meshTextures;

    for(auto &elem : meshTexturesSet) {
        meshTextures.push_back(elem);
    }

    std::vector <FloatRect> meshTexCoords;
    auto *meshTextureAtlas = m_device.getResourcesManager().getPackedTexture(meshTextures, meshTexCoords);

    if(!meshTextureAtlas) {
        int count{to - from + 1};
        int leftCount{count / 2};
        int rightCount{count - leftCount};

        if(!leftCount || !rightCount)
            return false;

        bool leftStatus{packTextures(meshes, from, from + leftCount - 1, batchTag)};

        if(!leftStatus)
            return false;

        bool rightStatus{packTextures(meshes, from + leftCount, from + leftCount + rightCount - 1, batchTag)};

        if(!rightStatus)
            return false;
    }
    else {
        E_RASSERT(meshTexCoords.size() == meshTextures.size(),
                  "Mesh tex coords count does not equal mesh textures count.");

        m_batches.resize(m_batches.size() + 1);

        auto &back = m_batches.back();
        auto matType = wantsTransparency ? irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF : irr::video::EMT_SOLID;

        back.reserve(10);
        back.push_back(new irrNodes::MeshBatch{*m_device.getIrrDevice().getSceneManager(),
                                               *meshTextureAtlas,
                                               matType,
                                               matType});

        // it's added to the Irrlicht scene manager, so we can drop it
        back.back()->drop();

        for(int i = from; i <= to; ++i) {
            auto &m = m_registeredMeshes[std::make_pair(meshes[i], batchTag)];

            m.batchIndex = m_batches.size() - 1;

            for(irr::u32 j = 0; j < meshes[i]->getMeshBufferCount(); ++j) {
                auto *meshTex = meshes[i]->getMeshBuffer(j)->getMaterial().TextureLayer[0].Texture;

                if(!meshTex)
                    m.textureAtlasRects.emplace_back();
                else {
                    bool found{};

                    for(size_t k = 0; k < meshTextures.size(); ++k) {
                        if(meshTex == meshTextures[k]) {
                            m.textureAtlasRects.push_back(meshTexCoords[k]);
                            found = true;
                            break;
                        }
                    }

                    E_DASSERT(found, "No texture found.");
                }
            }
        }
    }

    return true;
}

const int MeshBatchManager::k_maxVerticesPerBatch{10000};

} // namespace app3D
} // namespace engine
