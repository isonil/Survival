#ifndef ENGINE_APP_3D_WATER_HPP
#define ENGINE_APP_3D_WATER_HPP

#include "../../util/Vec3.hpp"
#include "../../util/Trace.hpp"
#include "SceneNode.hpp"

#include <irrlicht/irrlicht.h>

#include <memory>
#include <string>

namespace engine
{
namespace app3D
{

class TerrainDef;

class Water : public SceneNode, public Tracked <Water>
{
public:
    Water(const std::shared_ptr <TerrainDef> &terrainDef, const std::weak_ptr <Device> &device);

    void dropIrrObjects() override;
    void reloadIrrObjects() override;
    bool wantsEverUpdate() const override;

    void setPosition(const FloatVec3 &pos);
    const FloatVec3 &getPosition() const;
    void setScale(const FloatVec3 &scale);
    const FloatVec3 &getScale() const;

    ~Water() override;

private:
    struct
    {
        irr::scene::ISceneNode *waterNode{};
        bool deferredRendering{};
    } m_currentRender;

    void createRender();
    void removeCurrentRender();
    void updateCurrentRenderPosition();
    void updateCurrentRenderScale();

    static const std::string k_waterTexturePath;
    static const std::string k_foamTexturePath;

    FloatVec3 m_pos;
    FloatVec3 m_scale;
    std::shared_ptr <TerrainDef> m_terrainDef;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_WATER_HPP
