#ifndef ENGINE_APP_3D_TERRAIN_HPP
#define ENGINE_APP_3D_TERRAIN_HPP

#include "../../util/Trace.hpp"
#include "../../util/Vec2.hpp"
#include "../../util/Vec3.hpp"
#include "SceneNode.hpp"

#include <irrlicht/irrlicht.h>

#include <memory>
#include <string>

namespace engine { class WindGenerator; }

namespace engine
{
namespace app3D
{

namespace irrNodes { class GrassPatch; }

class TerrainDef;

class Terrain : public SceneNode, public Tracked <Terrain>
{
public:
    Terrain(const std::shared_ptr <TerrainDef> &terrainDef, const std::weak_ptr <Device> &device);

    void dropIrrObjects() override;
    void reloadIrrObjects() override;
    bool wantsEverUpdate() const override;

    bool isFlat() const;
    std::vector <float> getHeightGrid() const;
    void setPosition(const FloatVec3 &pos);
    const FloatVec3 &getPosition() const;
    float getHeight(const FloatVec2 &pos) const;
    TerrainDef &getDef() const;
    const std::shared_ptr <TerrainDef> &getDefPtr() const;

    ~Terrain() override;

private:
    struct
    {
        irr::scene::ITerrainSceneNode *terrainNode{};
        irr::scene::ISceneNode *terrainNode_helper{};
        irr::scene::ISceneNode *flatTerrainNode{};
        bool deferredRendering{};
        std::vector <irrNodes::GrassPatch*> grassPatches;
    } m_currentRender;

    void createRender();
    void removeCurrentRender();
    void updateCurrentRenderPosition();
    void updateCurrentRenderMaterial();

    static const int k_anisotropicFilterLevel;
    static const std::string k_causticsTexturePath;
    static const float k_windStrength;
    static const float k_windRegularity;
    static const std::string k_grassTexturePath;
    static const IntVec2 k_grassTexturesInTexture;
    static const IntRange k_slopeDistortionNormalMapBlueColorTransitionRange;

    std::shared_ptr <TerrainDef> m_terrainDef;
    FloatVec3 m_pos;

    std::shared_ptr <WindGenerator> m_windGenerator;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_TERRAIN_HPP
