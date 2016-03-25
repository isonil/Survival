#ifndef ENGINE_APP_3D_SCENE_MANAGER_HPP
#define ENGINE_APP_3D_SCENE_MANAGER_HPP

#include "../../util/Trace.hpp"
#include "../../util/Vec3.hpp"
#include "../../util/Color.hpp"
#include "../IIrrlichtObjectsHolder.hpp"

#include <irrlicht/irrlicht.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace engine { class AppTime; }

namespace engine
{
namespace app3D
{

class Device;
class Model;
class Terrain;
class Water;
class Light;
class ModelDef;
class TerrainDef;
class SceneNode;
class LightDef;
class Island;

class SceneManager : public IIrrlichtObjectsHolder, public Tracked <SceneManager>
{
public:
    SceneManager(Device &device);

    void setAmbientLight(float intensity);
    irr::scene::ICameraSceneNode &getIrrCamera();
    void setCameraPosition(const FloatVec3 &pos);
    FloatVec3 getCameraPosition() const;
    FloatVec3 getCameraRotation() const;
    FloatVec3 getCameraLookVec() const;
    float getAmbientLight() const;
    void setFOVMultiplier(float multiplier);
    float getFOV() const;

    void dropIrrObjects() override;
    void reloadIrrObjects() override;

    void update(const AppTime &appTime);
    void drawHighlightedNode();
    void drawFPPModels();
    void prepareToHighlightMesh(irr::scene::IMesh &mesh, const FloatVec3 &pos, const FloatVec3 &rot, float scale);

    void setLineMaterial();
    void draw3DLine(const FloatVec3 &from, const FloatVec3 &to, const Color &color) const;

    std::shared_ptr <Model> addModel(const std::shared_ptr <ModelDef> &modelDef, bool isFPP = false);
    std::shared_ptr <Terrain> addTerrain(const std::shared_ptr <TerrainDef> &terrainDef);
    std::shared_ptr <Water> addWater(const std::shared_ptr <TerrainDef> &terrainDef);
    std::shared_ptr <Light> addLight(const std::shared_ptr <LightDef> &lightDef);
    std::shared_ptr <Island> addIsland();

    static const float k_cameraFarValue;

private:
    struct
    {
        irr::scene::IMeshSceneNode *sceneNode_rendered{};
        irr::scene::IMesh *mesh_toRender{};
        FloatVec3 pos_toRender;
        FloatVec3 rot_toRender;
        float scale_toRender{1.f};
    } m_highlightedNode;

    struct
    {
        irr::video::SMaterial material;
        irr::video::S3DVertex vertices[4]{};
        irr::u16 indices[6]{};
    } m_fullScreenQuad;

    void initFullScreenQuad();
    void addSceneNode(const std::shared_ptr <SceneNode> &sceneNode);
    void drawFullScreenQuad(irr::video::E_MATERIAL_TYPE materialType, irr::video::ITexture &texture);

    static const float k_cameraRotateSpeed;
    static const float k_cameraNearValue;
    static const std::string k_skyBoxTexturesPath;

    Device &m_device;
    irr::scene::ICameraSceneNode *m_irrCamera;
    irr::scene::ISceneNode *m_irrSkyBox;
    irr::video::ITexture *m_highlightBufferRenderTarget;
    std::vector <std::shared_ptr <SceneNode>> m_sceneNodes_wantUpdate;
    std::vector <std::shared_ptr <SceneNode>> m_sceneNodes_dontWantUpdate;
    float m_ambientLight;
    float m_FOVMultiplier;
    irr::video::SMaterial m_lineMaterial;
    std::vector <std::weak_ptr <Model>> m_FPPModels;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_SCENE_MANAGER_HPP
