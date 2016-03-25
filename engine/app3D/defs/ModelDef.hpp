#ifndef ENGINE_APP_3D_MODEL_DEF_HPP
#define ENGINE_APP_3D_MODEL_DEF_HPP

#include "../../util/Trace.hpp"
#include "../../util/Enum.hpp"
#include "../../util/Rect.hpp"
#include "ResourceDef.hpp"

#include <irrlicht/irrlicht.h>

namespace engine
{
namespace app3D
{

class ModelDef : public ResourceDef, public Tracked <ModelDef>
{
public:
    ENUM_DECL(RenderTechnique,
        None,
        Mesh,
        AnimatedMesh,
        Billboard,
        HorizontalBillboard,
        MeshBatched,
        BillboardBatched,
        HorizontalBillboardBatched
    );

    class LOD : public DataFile::Saveable, public Tracked <LOD>
    {
        friend class ModelDef;

    public:
        LOD();
        LOD(const LOD &) = delete;
        LOD(LOD &&) = default;

        LOD &operator = (const LOD &) = delete;
        LOD &operator = (LOD &&) = default;

        void expose(DataFile::Node &node) override;

        int getIndex() const;
        RenderTechnique getRenderTechnique() const;
        const std::string &getBatchTag() const;
        float getScale() const;
        float getDistance() const;
        float getNextLODDistance() const;
        bool hasIrrMesh() const;
        irr::scene::IMesh &getIrrMesh() const;
        irr::scene::IAnimatedMesh &getIrrAnimatedMesh() const;
        irr::video::ITexture &getIrrBillboardTexture() const;
        bool getForceAllUpNormalsWhenBatched() const;
        bool isBillboardOverlay() const;
        bool getUseCenterAsOriginForBillboard() const;

    private:
        static const float k_maxLODDistance;

        int m_index;
        std::string m_resourcePath;
        RenderTechnique m_renderTechnique;
        std::string m_batchTag;
        float m_scale;
        float m_distance;
        float m_nextLODDistance;
        irr::scene::IMesh *m_irrMesh;
        irr::scene::IAnimatedMesh *m_irrAnimatedMesh;
        irr::video::ITexture *m_irrBillboardTexture;
        bool m_forceAllUpNormalsWhenBatched;
        bool m_isBillboardOverlay;
        bool m_useCenterAsOriginForBillboard;
    };

    void expose(DataFile::Node &node) override;

    void dropIrrObjects() override;
    void reloadIrrObjects() override;

    const LOD &getLOD(float distanceSq) const;
    const std::vector <LOD> &getLODs() const;

private:
    using base = ResourceDef;

    std::vector <LOD> m_LODs;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_MODEL_DEF_HPP
