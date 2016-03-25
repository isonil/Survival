#ifndef ENGINE_APP_3D_MESH_BATCH_MANAGER_HPP
#define ENGINE_APP_3D_MESH_BATCH_MANAGER_HPP

#include "../../util/Trace.hpp"
#include "../../util/Rect.hpp"
#include "../../util/Vec3.hpp"

#include <irrlicht/irrlicht.h>

#include <map>
#include <vector>

namespace engine { namespace app3D { namespace irrNodes { class MeshBatch; } } }

namespace engine
{
namespace app3D
{

class Device;

class MeshBatchManager : public Tracked <MeshBatchManager>
{
public:
    MeshBatchManager(Device &device);

    void registerMesh(irr::scene::IMesh &mesh, const std::string &batchTag);
    std::shared_ptr <int> addMesh(irr::scene::IMesh &mesh, const std::string &batchTag, bool forceAllUpNormals);
    void setMeshPosition(int index, const FloatVec3 &pos);
    void setMeshRotation(int index, const FloatVec3 &rot);
    void setMeshScale(int index, const FloatVec3 &scale);
    void removeMesh(int index);

private:
    struct RegisteredMesh
    {
        int batchIndex{-1};
        std::vector <FloatRect> textureAtlasRects;
    };

    struct BatchedMeshInfo
    {
        irr::scene::IMesh *mesh{};
        std::string batchTag;
        std::shared_ptr <int> index;
        int batchIndex{-1};
        int batchSecondLevelIndex{-1};
        std::shared_ptr <int> batchedMeshIndex;
        bool forceAllUpNormals{};
    };

    void createBatches();
    bool packTextures(const std::vector <irr::scene::IMesh *> &meshes, int from, int to, const std::string &batchTag);

    static const int k_maxVerticesPerBatch;

    Device &m_device;
    std::map <std::pair <irr::scene::IMesh *, std::string>, RegisteredMesh> m_registeredMeshes;
    std::vector <BatchedMeshInfo> m_batchedMeshes;
    std::vector <std::vector <irrNodes::MeshBatch *>> m_batches;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_MESH_BATCH_MANAGER_HPP
