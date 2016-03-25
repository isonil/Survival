#include "BillboardBatch.hpp"

#include "../../util/LogManager.hpp"
#include "../../util/Math.hpp"
#include "../../util/Trace.hpp"

namespace engine
{
namespace app3D
{
namespace irrNodes
{

BillboardBatch::BillboardBatch(irr::scene::ISceneManager &sceneManager, irr::video::ITexture &textureAtlas, irr::video::E_MATERIAL_TYPE defaultMaterialType, irr::video::E_MATERIAL_TYPE defaultDeferredRenderingMaterialType)
    :   ISceneNode(sceneManager.getRootSceneNode(), &sceneManager),
        m_radius{},
        m_deferredRendering{},
        m_defaultMaterialType{defaultMaterialType},
        m_defaultDeferredRenderingMaterialType{defaultDeferredRenderingMaterialType},
        m_updateNextFrame{true},
        m_frameCounter{}
{
    TRACK;

    m_material.TextureLayer[0].Texture = &textureAtlas;

    updateMaterial(false); // TODO: bool depending on current deferred status (pass to constructor?)
}

void BillboardBatch::OnRegisterSceneNode()
{
    TRACK;

    SceneManager->registerNodeForRendering(this, irr::scene::ESNRP_TRANSPARENT);
}

void BillboardBatch::render()
{
    TRACK;

    ++m_frameCounter;
    if(m_frameCounter == k_updateFrameFreq)
        m_updateNextFrame = true;

    if(m_updateNextFrame) {
        m_updateNextFrame = false;
        m_frameCounter = 0;
        updateBillboards();
    }

    auto &driver = *SceneManager->getVideoDriver();

    driver.setTransform(irr::video::ETS_WORLD, irr::core::matrix4{});
    driver.setMaterial(m_material);
    driver.drawMeshBuffer(&m_meshBuffer);
}

irr::video::SMaterial &BillboardBatch::getMaterial(irr::u32)
{
    return m_material;
}

irr::u32 BillboardBatch::getMaterialCount() const
{
    return 1;
}

const irr::core::aabbox3df &BillboardBatch::getBoundingBox() const
{
    // TODO: find a better way to define infinite bounding box? m_boundingBox can be removed probably
    static const irr::core::aabbox3df box{{-10000.f, -10000.f, -10000.f}, {10000.f, 10000.f, 10000.f}};
    return box;
}

void BillboardBatch::updateMaterial(bool useDeferredRendering)
{
    TRACK;

    if(useDeferredRendering) {
        m_deferredRendering = true;
        m_material.MaterialType = m_defaultDeferredRenderingMaterialType;
        m_material.Lighting = false;
        m_material.FogEnable = false;
    }
    else {
        m_deferredRendering = false;
        m_material.MaterialType = m_defaultMaterialType;
        m_material.Lighting = false; // TODO: true
        m_material.FogEnable = true; // TODO: true
    }
}

std::shared_ptr <int> BillboardBatch::addBillboard(const FloatRect &textureAtlasRect, const irr::video::SColor &vertexColor)
{
    TRACK;

    if(static_cast <int> (m_meshBuffer.Vertices.size() + 4) > k_maxVertices)
        throw Exception{"Billboard batch has reached maximum vertex count."};

    m_billboards.push_back(Billboard{});
    m_billboards.getLast().color = vertexColor;

    auto vertexIndex = static_cast <irr::s32> (m_meshBuffer.Vertices.size());

    m_radius = m_boundingBox.getExtent().getLength() / 2.f;

    float tx{textureAtlasRect.pos.x};
    float ty{textureAtlasRect.pos.y};
    float tw{textureAtlasRect.size.x};
    float th{textureAtlasRect.size.y};

    m_meshBuffer.Vertices.push_back({irr::core::vector3df{}, irr::core::vector3df{0.f, 1.f, 0.f}, vertexColor, irr::core::vector2df{tx, ty}});
    m_meshBuffer.Vertices.push_back({irr::core::vector3df{}, irr::core::vector3df{0.f, 1.f, 0.f}, vertexColor, irr::core::vector2df{tx + tw, ty}});
    m_meshBuffer.Vertices.push_back({irr::core::vector3df{}, irr::core::vector3df{0.f, 1.f, 0.f}, vertexColor, irr::core::vector2df{tx + tw, ty + th}});
    m_meshBuffer.Vertices.push_back({irr::core::vector3df{}, irr::core::vector3df{0.f, 1.f, 0.f}, vertexColor, irr::core::vector2df{tx, ty + th}});

    /*
        Vertices placement:
            0---1
            |   |
            3---2
    */

    m_meshBuffer.Indices.push_back(vertexIndex);
    m_meshBuffer.Indices.push_back(vertexIndex + 1);
    m_meshBuffer.Indices.push_back(vertexIndex + 2);

    m_meshBuffer.Indices.push_back(vertexIndex + 2);
    m_meshBuffer.Indices.push_back(vertexIndex + 3);
    m_meshBuffer.Indices.push_back(vertexIndex);

    auto index = static_cast <int> (m_billboards.size() - 1);

    m_billboards.getLast().index = std::make_shared <int> (index);
    m_billboardSortingArray.resize(m_billboards.size());
    m_billboardSortingCompareArray.resize(m_billboards.size());

    m_updateNextFrame = true;

    return m_billboards.getLast().index;
}

std::shared_ptr <int> BillboardBatch::addHorizontalBillboard(const FloatRect &textureAtlasRect, const irr::video::SColor &vertexColor)
{
    TRACK;

    const auto &index = addBillboard(textureAtlasRect, vertexColor);

    E_DASSERT(index, "Could not add horizontal billboard. Index pointer is nullptr.");
    E_DASSERT(*index >= 0 && static_cast <irr::u32> (*index) < m_billboards.size(), "Index out of bounds.");

    m_billboards[*index].horizontal = true;

    return index;
}

void BillboardBatch::setBillboardPosition(int index, const irr::core::vector3df &pos)
{
    if(index < 0 || static_cast <irr::u32> (index) >= m_billboards.size())
        return;

    m_billboards[index].pos = pos;
}

void BillboardBatch::setBillboardScale(int index, const irr::core::dimension2df &scale)
{
    if(index < 0 || static_cast <irr::u32> (index) >= m_billboards.size())
        return;

    m_billboards[index].scale = scale;
}

void BillboardBatch::removeBillboard(int index)
{
    TRACK;

    if(index < 0 || static_cast <irr::u32> (index) >= m_billboards.size())
        return;

    E_DASSERT(m_meshBuffer.Indices.size() == m_billboards.size() * 6, "Indices size is not billboards count * 6.");
    E_DASSERT(m_meshBuffer.Vertices.size() == m_billboards.size() * 4, "Vertices size is not billboards count * 4.");

    irr::u32 lastIndex{m_billboards.size() - 1};

    std::swap(m_billboards[index], m_billboards[lastIndex]);

    E_DASSERT(m_billboards[index].index, "Index is nullptr.");
    E_DASSERT(m_billboards[lastIndex].index, "Index is nullptr.");

    *m_billboards[lastIndex].index = -1;
    m_billboards.erase(lastIndex);

    if(static_cast <irr::u32> (index) != lastIndex)
        *m_billboards[index].index = index;

    for(int i = 0; i < 6; ++i) {
        m_meshBuffer.Indices.erase(m_meshBuffer.Indices.size() - 1);
    }

    std::swap(m_meshBuffer.Vertices[index * 4], m_meshBuffer.Vertices[m_meshBuffer.Vertices.size() - 4]);
    std::swap(m_meshBuffer.Vertices[index * 4 + 1], m_meshBuffer.Vertices[m_meshBuffer.Vertices.size() - 3]);
    std::swap(m_meshBuffer.Vertices[index * 4 + 2], m_meshBuffer.Vertices[m_meshBuffer.Vertices.size() - 2]);
    std::swap(m_meshBuffer.Vertices[index * 4 + 3], m_meshBuffer.Vertices[m_meshBuffer.Vertices.size() - 1]);

    for(int i = 0; i < 4; ++i) {
        m_meshBuffer.Vertices.erase(m_meshBuffer.Vertices.size() - 1);
    }

    E_DASSERT(!m_billboardSortingArray.empty(), "Billboard sorting array is empty.");
    E_DASSERT(!m_billboardSortingCompareArray.empty(), "Billboard sorting compare array is empty.");

    m_billboardSortingArray.resize(m_billboards.size());
    m_billboardSortingCompareArray.resize(m_billboards.size());

    m_updateNextFrame = true;
}

int BillboardBatch::getBillboardCount() const
{
    return m_billboards.size();
}

void BillboardBatch::applyVertexShadows(const irr::core::vector3df &lightDir, irr::f32 intensity, irr::f32 ambient)
{
    TRACK;

    E_DASSERT(m_meshBuffer.Vertices.size() == m_billboards.size() * 4, "Vertices size is not billboards count * 4.");

    // TODO: either remove, or make it actually useful

    for(irr::u32 i = 0; i < m_billboards.size(); ++i) {
        irr::core::vector3df normal{m_billboards[i].pos};
        normal.normalize();

        irr::f32 light{-lightDir.dotProduct(normal) * intensity + ambient};

        light = Math::clamp(light, 0.f, 1.f);

        irr::video::SColor color;

        color.setRed(static_cast <irr::u8> (m_billboards[i].color.getRed() * light));
        color.setGreen(static_cast <irr::u8> (m_billboards[i].color.getGreen() * light));
        color.setBlue(static_cast <irr::u8> (m_billboards[i].color.getBlue() * light));
        color.setAlpha(m_billboards[i].color.getAlpha());

        for(irr::s32 j = 0; j < 4; ++j) {
            m_meshBuffer.Vertices[i * 4 + j].Color = color;
        }
    }
}

void BillboardBatch::resetVertexShadows()
{
    TRACK;

    E_DASSERT(m_meshBuffer.Vertices.size() == m_billboards.size() * 4, "Vertices size is not billboards count * 4.");

    for(irr::u32 i = 0; i < m_billboards.size(); ++i) {
        for(irr::s32 j = 0; j < 4; ++j) {
            m_meshBuffer.Vertices[i * 4 + j].Color = m_billboards[i].color;
        }
    }
}

BillboardBatch::BillboardSortComparator::BillboardSortComparator(const std::vector <float> &compare)
    : m_compare{compare}
{
}

bool BillboardBatch::BillboardSortComparator::operator () (int a, int b) const
{
    return m_compare[a] > m_compare[b];
}

void BillboardBatch::updateBillboards()
{
    TRACK;

    E_DASSERT(m_billboardSortingArray.size() == m_billboards.size(),
              "Billboard sorting array does not have the same size as billboards count.");
    E_DASSERT(m_billboardSortingCompareArray.size() == m_billboards.size(),
              "Billboard sorting compare array does not have the same size as billboards count.");

    E_DASSERT(m_meshBuffer.Indices.size() == m_billboards.size() * 6, "Indices size is not billboards count * 6.");
    E_DASSERT(m_meshBuffer.Vertices.size() == m_billboards.size() * 4, "Vertices size is not billboards count * 4.");

    E_DASSERT(SceneManager->getActiveCamera(), "No active camera.");

    const auto &camPos = SceneManager->getActiveCamera()->getPosition();

    for(irr::u32 i = 0; i < m_billboards.size(); ++i) {
        auto distance = camPos.getDistanceFromSQ(m_billboards[i].pos);

        m_billboardSortingArray[i] = i;
        m_billboardSortingCompareArray[i] = distance;
    }

    BillboardSortComparator billboardSortComparator{m_billboardSortingCompareArray};
    std::sort(m_billboardSortingArray.begin(), m_billboardSortingArray.end(), billboardSortComparator);

    for(size_t i = 0; i < m_billboardSortingArray.size(); ++i) {
        auto indexBase = i * 6;
        auto valueBase = m_billboardSortingArray[i] * 4;

        m_meshBuffer.Indices[indexBase] = valueBase;
        m_meshBuffer.Indices[indexBase + 1] = valueBase + 1;
        m_meshBuffer.Indices[indexBase + 2] = valueBase + 2;
        m_meshBuffer.Indices[indexBase + 3] = valueBase + 2;
        m_meshBuffer.Indices[indexBase + 4] = valueBase + 3;
        m_meshBuffer.Indices[indexBase + 5] = valueBase;
    }

    for(irr::u32 i = 0; i < m_billboards.size(); ++i) {
        auto vertexIndex = static_cast <irr::s32> (i * 4);

        irr::core::vector3df billPos{m_billboards[i].pos};

        AbsoluteTransformation.transformVect(billPos);

        irr::core::vector3df cameraDiff;

        if(m_billboards[i].horizontal)
            cameraDiff = m_billboards[i].pos - irr::core::vector3df{camPos.X, m_billboards[i].pos.Y, camPos.Z};
        else
            cameraDiff = m_billboards[i].pos - camPos;

        irr::core::vector3df upVector{0.f, 1.f, 0.f};
        irr::core::vector3df crossA{cameraDiff.crossProduct(upVector).normalize()};
        irr::core::vector3df crossB{cameraDiff.crossProduct(crossA).normalize()};

        crossA *= m_billboards[i].scale.Width * 0.5f;
        crossB *= m_billboards[i].scale.Height * 0.5f;

        m_meshBuffer.Vertices[vertexIndex].Pos = billPos + crossA - crossB;
        m_meshBuffer.Vertices[vertexIndex + 1].Pos = billPos - crossA - crossB;
        m_meshBuffer.Vertices[vertexIndex + 2].Pos = billPos - crossA + crossB;
        m_meshBuffer.Vertices[vertexIndex + 3].Pos = billPos + crossA + crossB;
    }
}

const int BillboardBatch::k_maxVertices{50000};
const int BillboardBatch::k_updateFrameFreq{5};

} // namespace irrNodes
} // namespace app3D
} // namespace engine
