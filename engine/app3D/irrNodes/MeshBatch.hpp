#ifndef ENGINE_APP_3D_IRR_NODES_MESH_BATCH_HPP
#define ENGINE_APP_3D_IRR_NODES_MESH_BATCH_HPP

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

class MeshBatch : public irr::scene::ISceneNode, public Tracked <MeshBatch>
{
public:
    MeshBatch(irr::scene::ISceneManager &sceneManager, irr::video::ITexture &textureAtlas, irr::video::E_MATERIAL_TYPE defaultMaterialType, irr::video::E_MATERIAL_TYPE defaultDeferredRenderingMaterialType);

    void OnRegisterSceneNode() override;
    void render() override;
    irr::video::SMaterial &getMaterial(irr::u32) override;
    irr::u32 getMaterialCount() const override;
    const irr::core::aabbox3df &getBoundingBox() const override;
    void updateMaterial(bool useDeferredRendering);
    std::shared_ptr <int> addMesh(irr::scene::IMesh &mesh, const std::vector <FloatRect> &textureAtlasRects, bool forceAllUpNormals);
    void setMeshPosition(int index, const irr::core::vector3df &pos);
    void setMeshRotation(int index, const irr::core::vector3df &rot);
    void setMeshScale(int index, const irr::core::vector3df &scale);
    void removeMesh(int index);
    int getMeshCount() const;
    int getVertexCount() const;
    irr::video::E_MATERIAL_TYPE getDefaultMaterialType() const;
    irr::video::E_MATERIAL_TYPE getDefaultDeferredRenderingMaterialType() const;

private:
    struct Mesh
    {
        Mesh();

        std::shared_ptr <int> index;
        irr::core::vector3df pos, rot, scale;
        int verticesStartIndex;
        int verticesEndIndex;
        irr::core::matrix4 rotationMatrix;
        std::vector <irr::video::S3DVertex> vertices;
        std::vector <irr::u16> indices;
    };

    static void transformVertex(irr::core::vector3df &ver, irr::core::vector3df &normal, const Mesh &mesh);

    void updateMeshes();

    static int k_maxVertices;

    bool m_deferredRendering;
    irr::video::E_MATERIAL_TYPE m_defaultMaterialType;
    irr::video::E_MATERIAL_TYPE m_defaultDeferredRenderingMaterialType;
    bool m_recalculateMeshBuffer;
    bool m_reportedBadTexCoords;
    std::vector <Mesh> m_meshes;
    irr::video::SMaterial m_material;
    irr::scene::SMeshBuffer m_meshBuffer;
};

} // namespace irrNodes
} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_IRR_NODES_MESH_BATCH_HPP
