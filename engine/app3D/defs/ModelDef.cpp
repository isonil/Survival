#include "ModelDef.hpp"

#include "../managers/ResourcesManager.hpp"
#include "../Device.hpp"

namespace engine
{
namespace app3D
{

ENUM_DEF(ModelDef::RenderTechnique, RenderTechnique,
    None,
    Mesh,
    AnimatedMesh,
    Billboard,
    HorizontalBillboard,
    MeshBatched,
    BillboardBatched,
    HorizontalBillboardBatched
);

ModelDef::LOD::LOD()
    : m_index{},
      m_resourcePath{"undefined"},
      m_renderTechnique{RenderTechnique::None},
      m_scale{1.f},
      m_distance{},
      m_nextLODDistance{k_maxLODDistance + 1.f},
      m_irrMesh{},
      m_irrAnimatedMesh{},
      m_irrBillboardTexture{},
      m_forceAllUpNormalsWhenBatched{},
      m_isBillboardOverlay{},
      m_useCenterAsOriginForBillboard{}
{
}

void ModelDef::LOD::expose(DataFile::Node &node)
{
    node.var(m_resourcePath, "resourcePath");
    node.var(m_renderTechnique, "renderTechnique");
    node.var(m_batchTag, "batchTag", "");
    node.var(m_scale, "scale", 1.f);
    node.var(m_distance, "distance", {});
    node.var(m_forceAllUpNormalsWhenBatched, "forceAllUpNormalsWhenBatched", {});
    node.var(m_isBillboardOverlay, "isBillboardOverlay", {});
    node.var(m_useCenterAsOriginForBillboard, "useCenterAsOriginForBillboard", {});

    if(node.getActivityType() == DataFile::Activity::Type::Loading) {
        if(m_scale < 0.f)
            throw Exception{"Scale can't be negative."};

        if(m_distance < 0.f)
            throw Exception{"Distance can't be negative."};
    }
}

int ModelDef::LOD::getIndex() const
{
    return m_index;
}

ModelDef::RenderTechnique ModelDef::LOD::getRenderTechnique() const
{
    return m_renderTechnique;
}

const std::string &ModelDef::LOD::getBatchTag() const
{
    return m_batchTag;
}

float ModelDef::LOD::getScale() const
{
    return m_scale;
}

float ModelDef::LOD::getDistance() const
{
    return m_distance;
}

float ModelDef::LOD::getNextLODDistance() const
{
    return m_nextLODDistance;
}

bool ModelDef::LOD::hasIrrMesh() const
{
    return m_irrMesh;
}

irr::scene::IMesh &ModelDef::LOD::getIrrMesh() const
{
    TRACK;

    if(!m_irrMesh) {
        throw Exception{"Irrlicht mesh from ModelDef is nullptr. "
                              "Render technique is \"" + m_renderTechnique.toString() + "\"."};
    }

    return *m_irrMesh;
}

irr::scene::IAnimatedMesh &ModelDef::LOD::getIrrAnimatedMesh() const
{
    TRACK;

    if(!m_irrAnimatedMesh) {
        throw Exception{"Irrlicht animated mesh from ModelDef is nullptr. "
                              "Render technique is \"" + m_renderTechnique.toString() + "\"."};
    }

    return *m_irrAnimatedMesh;
}

irr::video::ITexture &ModelDef::LOD::getIrrBillboardTexture() const
{
    TRACK;

    if(!m_irrBillboardTexture) {
        throw Exception{"Irrlicht billboard texture from ModelDef is nullptr. "
                              "Render technique is \"" + m_renderTechnique.toString() + "\"."};
    }

    return *m_irrBillboardTexture;
}

bool ModelDef::LOD::getForceAllUpNormalsWhenBatched() const
{
    return m_forceAllUpNormalsWhenBatched;
}

bool ModelDef::LOD::isBillboardOverlay() const
{
    return m_isBillboardOverlay;
}

bool ModelDef::LOD::getUseCenterAsOriginForBillboard() const
{
    return m_useCenterAsOriginForBillboard;
}

const float ModelDef::LOD::k_maxLODDistance{15000.f};

void ModelDef::expose(DataFile::Node &node)
{
    base::expose(node);

    node.var(m_LODs, "LODs");

    if(node.getActivityType() == DataFile::Activity::Type::Loading) {
        for(size_t i = 0; i < m_LODs.size(); ++i) {
            m_LODs[i].m_index = i;

            if(m_LODs[i].m_distance > LOD::k_maxLODDistance) {
                const auto &defName = getDefName();
                E_WARNING("Model \"%s\" has LOD distance greater than max LOD distance.", defName.c_str());
                m_LODs[i].m_distance = LOD::k_maxLODDistance;
            }

            if(i) {
                m_LODs[i - 1].m_nextLODDistance = m_LODs[i].m_distance;

                if(m_LODs[i - 1].m_distance > m_LODs[i - 1].m_nextLODDistance)
                    throw Exception{"LOD distance must be greater than previous LOD distance."};
            }
            else {
                if(!Math::fuzzyCompare(m_LODs[i].m_distance, 0.f))
                    throw Exception{"First LOD distance must be 0."};
            }
        }
    }
}

void ModelDef::dropIrrObjects()
{
    TRACK;

    for(auto &elem : m_LODs) {
        elem.m_irrMesh = nullptr;
        elem.m_irrAnimatedMesh = nullptr;
        elem.m_irrBillboardTexture = nullptr;
    }
}

void ModelDef::reloadIrrObjects()
{
    TRACK;

    auto &resourcesManager = getDevice_slow().getResourcesManager();

    for(auto &elem : m_LODs) {
        const auto &technique = elem.m_renderTechnique;

        if(technique == RenderTechnique::Mesh || technique == RenderTechnique::MeshBatched) {
            elem.m_irrMesh = &resourcesManager.loadIrrMesh(elem.m_resourcePath);

            if(technique == RenderTechnique::MeshBatched)
                resourcesManager.getMeshBatchManager().registerMesh(*elem.m_irrMesh, elem.m_batchTag);
        }
        else if(technique == RenderTechnique::AnimatedMesh)
            elem.m_irrAnimatedMesh = &resourcesManager.loadIrrAnimatedMesh(elem.m_resourcePath);
        else if(technique == RenderTechnique::Billboard || technique == RenderTechnique::HorizontalBillboard ||
                technique == RenderTechnique::BillboardBatched || technique == RenderTechnique::HorizontalBillboardBatched) {
            elem.m_irrBillboardTexture = &resourcesManager.loadIrrTexture(elem.m_resourcePath, true);

            if(technique == RenderTechnique::BillboardBatched ||
               technique == RenderTechnique::HorizontalBillboardBatched) {
                resourcesManager.getBillboardBatchManager().registerBillboard(*elem.m_irrBillboardTexture, elem.m_batchTag);
            }
        }
        else if(technique != RenderTechnique::None)
            throw Exception{"Render technique not handled (" + technique.toString() + ")."};
    }
}

const ModelDef::LOD &ModelDef::getLOD(float distanceSq) const
{
    TRACK;

    if(m_LODs.empty())
        throw Exception{"Model type has no LODs."};

    const auto &found = std::find_if(m_LODs.rbegin(), m_LODs.rend(), [&distanceSq](const LOD &lod) {
        auto lodDist = lod.getDistance();
        return distanceSq > lodDist * lodDist;
    });

    if(found != m_LODs.rend())
        return *found;

    return m_LODs[0];
}

const std::vector <ModelDef::LOD> &ModelDef::getLODs() const
{
    return m_LODs;
}

} // namespace app3D
} // namespace engine
