#ifndef ENGINE_APP_3D_IRR_NODES_BILLBOARD_BATCH_HPP
#define ENGINE_APP_3D_IRR_NODES_BILLBOARD_BATCH_HPP

#include "../../util/Trace.hpp"
#include "../../util/Rect.hpp"

#include <irrlicht/irrlicht.h>

#include <memory>
#include <vector>

namespace engine
{
namespace app3D
{
namespace irrNodes
{

class BillboardBatch : public irr::scene::ISceneNode, public Tracked <BillboardBatch>
{
public:
    BillboardBatch(irr::scene::ISceneManager &sceneManager, irr::video::ITexture &textureAtlas, irr::video::E_MATERIAL_TYPE defaultMaterialType, irr::video::E_MATERIAL_TYPE defaultDeferredRenderingMaterialType);

    void OnRegisterSceneNode() override;
    void render() override;
    irr::video::SMaterial &getMaterial(irr::u32) override;
    irr::u32 getMaterialCount() const override;
    const irr::core::aabbox3df &getBoundingBox() const override;
    void updateMaterial(bool useDeferredRendering);
    std::shared_ptr <int> addBillboard(const FloatRect &textureAtlasRect, const irr::video::SColor &vertexColor = irr::video::SColor(255, 255, 255, 255));
    std::shared_ptr <int> addHorizontalBillboard(const FloatRect &textureAtlasRect, const irr::video::SColor &vertexColor = irr::video::SColor(255, 255, 255, 255));
    void setBillboardPosition(int index, const irr::core::vector3df &pos);
    void setBillboardScale(int index, const irr::core::dimension2df &scale);
    void removeBillboard(int index);
    int getBillboardCount() const;
    void applyVertexShadows(const irr::core::vector3df &lightDir, irr::f32 intensity, irr::f32 ambient);
    void resetVertexShadows();

private:
    struct Billboard
    {
        std::shared_ptr <int> index;
        irr::core::vector3df pos;
        irr::core::dimension2df scale{1.f, 1.f};
        irr::f32 roll{};
        irr::video::SColor color;
        bool horizontal{};
    };

    class BillboardSortComparator
    {
    public:
        BillboardSortComparator(const std::vector <float> &compare);

        bool operator () (int a, int b) const;

    private:
        const std::vector <float> &m_compare;
    };

    void updateBillboards();

    static const int k_maxVertices;
    static const int k_updateFrameFreq;

    irr::f32 m_radius;
    bool m_deferredRendering;
    irr::video::E_MATERIAL_TYPE m_defaultMaterialType;
    irr::video::E_MATERIAL_TYPE m_defaultDeferredRenderingMaterialType;
    irr::core::aabbox3df m_boundingBox;
    irr::video::SMaterial m_material;
    irr::core::array <Billboard> m_billboards;
    std::vector <int> m_billboardSortingArray;
    std::vector <float> m_billboardSortingCompareArray;
    irr::scene::SMeshBuffer m_meshBuffer;
    bool m_updateNextFrame;
    int m_frameCounter;
};

} // namespace irrNodes
} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_IRR_NODES_BILLBOARD_BATCH_HPP
