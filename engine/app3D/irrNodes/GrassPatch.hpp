#ifndef GRASS_PATCH_SCENE_NODE_HPP
#define GRASS_PATCH_SCENE_NODE_HPP

#include "../../util/Vec2.hpp"
#include "../../util/Vec3.hpp"
#include "../../util/Color.hpp"

#include <irrlicht.h>

#include <memory>

namespace engine { class WindGenerator; }

namespace engine
{
namespace app3D
{
namespace irrNodes
{

class GrassPatch : public irr::scene::ISceneNode
{
public:
    GrassPatch(irr::scene::ISceneManager &sceneManager, irr::scene::ITerrainSceneNode &terrain, irr::scene::ISceneNode &parent,
               const FloatVec2 &pos, const FloatVec2 &terrainSize,
               irr::video::IImage &normalMapImage, irr::video::IImage &splatMapImage,
               const Color &grassColor, const IntVec2 &texturesInTextureCount, const std::shared_ptr <WindGenerator> &windGenerator);

    irr::video::SMaterial &getMaterial(irr::u32 i) override;
    irr::u32 getMaterialCount() const override;
    const irr::core::aabbox3d <irr::f32> &getBoundingBox() const override;
    void OnRegisterSceneNode() override;
    void OnAnimate(irr::u32 timeMs) override;
    void render() override;

    static const float k_grassPatchSize;

private:
    struct GrassParticle
    {
        irr::video::SColor color;
        irr::core::vector3df pos;
        irr::core::dimension2d <irr::s32> sprite;
        irr::core::vector3df points[4];
        irr::f32 height{};
        irr::f32 flex{};
    };

    void setTexturesInTexture(const IntVec2 &count);
    void setWindRes(int res);
    void allocateBuffers();

    static const float k_defaultDrawDist;
    static const irr::u32 k_defaultTimeBetweenAnimationFrames;
    static const int k_grassQuadsCount;
    static const int k_splatMapGreenColorThreshold;
    static const int k_normalMapUpVectorThreshold;
    static const int k_windRes;
    static const float k_heightToFlexFactor;
    static const float k_colorMultiplierForBottomVertices;

    std::shared_ptr <WindGenerator> m_windGenerator;
    irr::core::vector3df m_pos;
    FloatVec2 m_terrainSize;
    Color m_grassColor;

    irr::scene::ITerrainSceneNode &m_terrain;
    irr::video::IImage &m_normalMapImage;
    irr::video::IImage &m_splatMapImage;

    float m_drawDistSq;

    std::vector <irr::core::vector2df> m_windGrid;
    int m_windGridRes;
    irr::u32 m_lastWindChangeTime;
    irr::u32 m_lastDrawCount;
    irr::u32 m_timeBetweenAnimationFrames;
    bool m_redrawNextLoop;

    irr::core::dimension2d <irr::s32> m_texturesInTextureCount;
    std::vector <GrassParticle> m_particles;

    irr::video::SMaterial m_material;
    std::vector <irr::video::S3DVertex> m_vertices;
    std::vector <irr::u16> m_indices;
    irr::core::aabbox3d <irr::f32> m_boundingBox;

    // texture coords for each grass image
    // (precalculated to avoid calculating them in each loop)
    std::vector <irr::f32> m_vertex1TexCoords;
    std::vector <irr::f32> m_vertex2TexCoords;
    std::vector <irr::f32> m_vertex3TexCoords;
    std::vector <irr::f32> m_vertex4TexCoords;
};

} // namespace irrNodes
} // namespace app3D
} // namespace engine

#endif
