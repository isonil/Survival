#include "BillboardBatchManager.hpp"

#include "../../util/Exception.hpp"
#include "../../util/LogManager.hpp"
#include "../irrNodes/BillboardBatch.hpp"
#include "../Device.hpp"
#include "../IrrlichtConversions.hpp"
#include "ResourcesManager.hpp"

#include <utility>

namespace engine
{
namespace app3D
{

BillboardBatchManager::BillboardBatchManager(Device &device)
    : m_device{device}
{
    m_batchedBillboards.reserve(500);
}

void BillboardBatchManager::registerBillboard(irr::video::ITexture &billboard, const std::string &batchTag)
{
    TRACK;

    const auto &key = std::make_pair(&billboard, batchTag);
    const auto &it = m_registeredBillboards.find(key);

    if(it == m_registeredBillboards.end())
        m_registeredBillboards.emplace(key, RegisteredBillboard{});
}

std::shared_ptr <int> BillboardBatchManager::addBillboard(irr::video::ITexture &billboard, const std::string &batchTag, bool horizontal)
{
    TRACK;

    auto &m = m_registeredBillboards[std::make_pair(&billboard, batchTag)];

    // if this billboard hasn't been added to any batch yet, then recreate all batches
    if(m.batchIndex < 0)
        createBatches();

    E_DASSERT(m.batchIndex >= 0 && static_cast <size_t> (m.batchIndex) < m_batches.size(), "Batch index out of bounds.");
    E_DASSERT(!m_batches[m.batchIndex].empty(), "There must be at least one batch available.");
    E_DASSERT(m_batches[m.batchIndex][0], "First batch is nullptr.");

    int batchSecondLevelIndex{-1};

    for(size_t i = 0; i < m_batches[m.batchIndex].size(); ++i) {
        E_DASSERT(m_batches[m.batchIndex][i], "Batch is nullptr.");

        if(m_batches[m.batchIndex][i]->getBillboardCount() < k_maxBillboardsPerBatch) {
            batchSecondLevelIndex = static_cast <int> (i);
            break;
        }
    }

    if(batchSecondLevelIndex < 0) {
        auto *tex = m_batches[m.batchIndex][0]->getMaterial(0).TextureLayer[0].Texture;

        E_DASSERT(tex, "Billboard batch texture is nullptr.");

        m_batches[m.batchIndex].push_back(new irrNodes::BillboardBatch{*m_device.getIrrDevice().getSceneManager(),
                                                                       *tex,
                                                                       irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL,
                                                                       irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL});

        // it's added to the Irrlicht scene manager, so we can drop it
        m_batches[m.batchIndex].back()->drop();
        batchSecondLevelIndex = static_cast <int> (m_batches[m.batchIndex].size()) - 1;
    }

    std::shared_ptr <int> batchedBillboardIndex;

    if(horizontal)
        batchedBillboardIndex = m_batches[m.batchIndex][batchSecondLevelIndex]->addHorizontalBillboard(m.textureAtlasRect);
    else
        batchedBillboardIndex = m_batches[m.batchIndex][batchSecondLevelIndex]->addBillboard(m.textureAtlasRect);

    m_batchedBillboards.emplace_back();

    auto &added = m_batchedBillboards.back();

    added.billboard = &billboard;
    added.batchTag = batchTag;
    added.batchIndex = m.batchIndex;
    added.batchSecondLevelIndex = batchSecondLevelIndex;
    added.batchedBillboardIndex = batchedBillboardIndex;
    added.horizontal = horizontal;
    added.index = std::make_shared <int> (m_batchedBillboards.size() - 1);

    return added.index;
}

void BillboardBatchManager::setBillboardPosition(int index, const FloatVec3 &pos)
{
    TRACK;

    if(index < 0 || static_cast <size_t> (index) >= m_batchedBillboards.size())
        return;

    auto &batchedBillboard = m_batchedBillboards[index];

    E_DASSERT(batchedBillboard.batchIndex >= 0 && static_cast <size_t> (batchedBillboard.batchIndex) < m_batches.size(),
              "Batched billboard batch index out of bounds.");

    E_DASSERT(batchedBillboard.batchedBillboardIndex, "Batched billboard index is nullptr.");

    E_DASSERT(batchedBillboard.batchSecondLevelIndex >= 0 && static_cast <size_t> (batchedBillboard.batchSecondLevelIndex) < m_batches[batchedBillboard.batchIndex].size(),
              "Batched billboard batch second level index out of bounds.");

    E_DASSERT(m_batches[batchedBillboard.batchIndex][batchedBillboard.batchSecondLevelIndex],
              "Batch is nullptr.");

    const auto &irrPos = IrrlichtConversions::toVector(pos);
    m_batches[batchedBillboard.batchIndex][batchedBillboard.batchSecondLevelIndex]->setBillboardPosition(*batchedBillboard.batchedBillboardIndex, irrPos);
}

void BillboardBatchManager::setBillboardScale(int index, const FloatVec2 &scale)
{
    TRACK;

    if(index < 0 || static_cast <size_t> (index) >= m_batchedBillboards.size())
        return;

    auto &batchedBillboard = m_batchedBillboards[index];

    E_DASSERT(batchedBillboard.batchIndex >= 0 && static_cast <size_t> (batchedBillboard.batchIndex) < m_batches.size(),
              "Batched billboard batch index out of bounds.");

    E_DASSERT(batchedBillboard.batchedBillboardIndex, "Batched billboard index is nullptr.");

    E_DASSERT(batchedBillboard.batchSecondLevelIndex >= 0 && static_cast <size_t> (batchedBillboard.batchSecondLevelIndex) < m_batches[batchedBillboard.batchIndex].size(),
              "Batched billboard batch second level index out of bounds.");

    E_DASSERT(m_batches[batchedBillboard.batchIndex][batchedBillboard.batchSecondLevelIndex],
              "Batch is nullptr.");

    const auto &irrScale = IrrlichtConversions::toDimension(scale);
    m_batches[batchedBillboard.batchIndex][batchedBillboard.batchSecondLevelIndex]->setBillboardScale(*batchedBillboard.batchedBillboardIndex, irrScale);
}

void BillboardBatchManager::removeBillboard(int index)
{
    TRACK;

    if(index < 0 || static_cast <size_t> (index) >= m_batchedBillboards.size())
        return;

    auto &batchedBillboard = m_batchedBillboards[index];

    E_DASSERT(batchedBillboard.batchIndex >= 0 && static_cast <size_t> (batchedBillboard.batchIndex) < m_batches.size(),
              "Batched billboard batch index out of bounds.");

    E_DASSERT(batchedBillboard.batchedBillboardIndex, "Batched billboard index is nullptr.");

    E_DASSERT(batchedBillboard.batchSecondLevelIndex >= 0 && static_cast <size_t> (batchedBillboard.batchSecondLevelIndex) < m_batches[batchedBillboard.batchIndex].size(),
              "Batched billboard batch second level index out of bounds.");

    E_DASSERT(m_batches[batchedBillboard.batchIndex][batchedBillboard.batchSecondLevelIndex],
              "Batch is nullptr.");

    m_batches[batchedBillboard.batchIndex][batchedBillboard.batchSecondLevelIndex]->removeBillboard(*batchedBillboard.batchedBillboardIndex);

    // batchedBillboard variable invalidated

    size_t lastIndex{m_batchedBillboards.size() - 1};
    std::swap(m_batchedBillboards[index], m_batchedBillboards[lastIndex]);

    E_DASSERT(m_batchedBillboards[index].index, "Index pointer is nullptr.");
    E_DASSERT(m_batchedBillboards[lastIndex].index, "Last index index pointer is nullptr.");

    *m_batchedBillboards[lastIndex].index = -1;
    m_batchedBillboards.pop_back();

    if(static_cast <size_t> (index) != lastIndex)
        *m_batchedBillboards[index].index = index;
}

void BillboardBatchManager::createBatches()
{
    TRACK;

    E_INFO("Creating billboard batches.");

    for(auto &elem : m_batchedBillboards) {
        E_DASSERT(elem.batchedBillboardIndex, "Batched billboard index is nullptr.");
        *elem.batchedBillboardIndex = -1;
    }

    for(auto &elem1 : m_batches) {
        for(auto &elem2 : elem1) {
            E_DASSERT(elem2, "Batch is nullptr.");
            elem2->remove();
        }
    }

    m_batches.clear();

    for(auto &elem : m_registeredBillboards) {
        elem.second.batchIndex = -1;
        elem.second.textureAtlasRect = FloatRect{};
    }

    if(!m_registeredBillboards.empty()) {
        std::set <std::string> batchTags;

        for(auto &elem : m_registeredBillboards) {
            batchTags.insert(elem.first.second);
        }

        for(auto &b : batchTags) {
            std::vector <irr::video::ITexture *> billboards;

            for(auto &m : m_registeredBillboards) {
                if(m.first.second == b)
                    billboards.push_back(m.first.first);
            }

            if(!billboards.empty()) {
                if(!packTextures(billboards, 0, billboards.size() - 1, b)) {
                    if(b.empty())
                        throw Exception{"Textures did not fit in packed textures."};
                    else
                        throw Exception{"Textures did not fit in packed textures (batch tag \"" + b + "\")."};
                }
            }
        }

        E_INFO("Billboard batches count: %d.", static_cast <int> (m_batches.size()));
    }

    for(auto &elem : m_batchedBillboards) {
        E_DASSERT(elem.billboard, "Billboard is nullptr.");
        elem.batchedBillboardIndex = addBillboard(*elem.billboard, elem.batchTag, elem.horizontal);
    }
}

bool BillboardBatchManager::packTextures(const std::vector <irr::video::ITexture *> &billboards, int from, int to, const std::string &batchTag)
{
    TRACK;

    E_DASSERT(to - from >= 0, "Empty range.");

    std::set <irr::video::ITexture *> billboardTexturesSet;

    for(int i = from; i <= to; ++i) {
        E_DASSERT(billboards[i], "Billboard is nullptr.");
        billboardTexturesSet.insert(billboards[i]);
    }

    std::vector <irr::video::ITexture *> billboardTextures;

    for(auto &elem : billboardTexturesSet) {
        billboardTextures.push_back(elem);
    }

    std::vector <FloatRect> billboardTexCoords;
    auto *billboardTextureAtlas = m_device.getResourcesManager().getPackedTexture(billboardTextures, billboardTexCoords);

    if(!billboardTextureAtlas) {
        int count{to - from + 1};
        int leftCount{count / 2};
        int rightCount{count - leftCount};

        if(!leftCount || !rightCount)
            return false;

        bool leftStatus{packTextures(billboards, from, from + leftCount - 1, batchTag)};

        if(!leftStatus)
            return false;

        bool rightStatus{packTextures(billboards, from + leftCount, from + leftCount + rightCount - 1, batchTag)};

        if(!rightStatus)
            return false;
    }
    else {
        E_RASSERT(billboardTexCoords.size() == billboardTextures.size(),
                  "Billboard tex coords count does not equal billboard textures count.");

        m_batches.emplace_back();

        auto &back = m_batches.back();

        back.reserve(10);
        back.push_back(new irrNodes::BillboardBatch{*m_device.getIrrDevice().getSceneManager(),
                                                    *billboardTextureAtlas,
                                                    irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL,
                                                    irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL});

        // it's added to the Irrlicht scene manager, so we can drop it
        back.back()->drop();

        for(int i = from; i <= to; ++i) {
            auto &m = m_registeredBillboards[std::make_pair(billboards[i], batchTag)];

            m.batchIndex = m_batches.size() - 1;

            bool found{};

            for(size_t j = 0; j < billboardTextures.size(); ++j) {
                if(billboards[i] == billboardTextures[j]) {
                    m.textureAtlasRect = billboardTexCoords[j];
                    found = true;
                    break;
                }
            }

            E_DASSERT(found, "No texture found.");
        }
    }

    return true;
}

const int BillboardBatchManager::k_maxBillboardsPerBatch{10000};

} // namespace app3D
} // namespace engine
