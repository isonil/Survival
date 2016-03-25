#ifndef ENGINE_APP_3D_RESOURCES_MANAGER_HPP
#define ENGINE_APP_3D_RESOURCES_MANAGER_HPP

#include "../../util/Trace.hpp"
#include "../../util/Exception.hpp"
#include "../managers/MeshBatchManager.hpp"
#include "../managers/BillboardBatchManager.hpp"
#include "../IIrrlichtObjectsHolder.hpp"
#include "../Settings.hpp"

#include <irrlicht/irrlicht.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace irr { namespace gui { class CGUITTFont; } }

namespace engine
{
namespace app3D
{

class ResourcesManager : public IIrrlichtObjectsHolder, public Tracked <ResourcesManager>
{
public:
    ResourcesManager(Device &device, const Settings &settings);
    ResourcesManager(const ResourcesManager &) = delete;

    ResourcesManager &operator = (const ResourcesManager &) = delete;

    void dropIrrObjects() override;
    void reloadIrrObjects() override;

    MeshBatchManager &getMeshBatchManager();
    BillboardBatchManager &getBillboardBatchManager();
    std::vector <FloatVec3> getMeshPoints(const std::string &meshPath, const std::string &preferredModPath = "core");

    std::string getPathToResource(const std::string &path, const std::string &preferredModPath = "core") const;

    irr::scene::IMesh &loadIrrMesh(const std::string &path, const std::string &preferredModPath = "core");
    irr::scene::IAnimatedMesh &loadIrrAnimatedMesh(const std::string &path, const std::string &preferredModPath = "core");
    irr::video::ITexture &loadIrrTexture(const std::string &path, bool generateMipMaps, const std::string &preferredModPath = "core");
    //irr::video::ITexture &loadIrrTextureArray(const std::vector <std::string> &texturePaths);
    irr::video::IImage &loadIrrImage(const std::string &path, const std::string &preferredModPath = "core");
    irr::gui::CGUITTFont &loadIrrFont(const std::string &path, int size, const std::string &preferredModPath = "core");

    irr::video::ITexture *getPackedTexture(const std::vector <irr::video::ITexture*> &textures, std::vector <FloatRect> &outTexCoords) const;
    irr::scene::IMesh &getSimplePlaneMesh() const;
    irr::video::ITexture &getWhiteTexture() const;
    irr::video::ITexture &getBlackTexture() const;

private:
    void processMeshMaterial(irr::video::SMaterial &material);

    irr::scene::IMesh *tryLoadIrrMesh_internal(const std::string &fullPath);
    irr::scene::IAnimatedMesh *tryLoadIrrAnimatedMesh_internal(const std::string &fullPath);
    irr::video::ITexture *tryLoadIrrTexture_internal(const std::string &fullPath);
    irr::video::IImage *tryLoadIrrImage_internal(const std::string &fullPath);
    irr::gui::CGUITTFont *tryLoadIrrFont_internal(const std::string &fullPath, int size);

    static const int k_packedTextureSize;
    static const int k_anisotropicFilterLevel;

    Device &m_device;
    const Settings m_settings;
    MeshBatchManager m_meshBatchManager;
    BillboardBatchManager m_billboardBatchManager;
    int m_uniqueResourceID;
    irr::scene::IAnimatedMesh *m_missingMeshMesh;
    irr::video::ITexture *m_missingTextureTexture;
    irr::video::IImage *m_missingImageImage;
    irr::gui::CGUITTFont *m_defaultFont;
    irr::video::ITexture *m_whiteTexture;
    irr::video::ITexture *m_blackTexture;
    irr::scene::IMesh *m_simplePlaneMesh;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_RESOURCES_MANAGER_HPP
