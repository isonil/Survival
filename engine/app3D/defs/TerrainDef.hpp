#ifndef ENGINE_APP_3D_TERRAIN_DEF_HPP
#define ENGINE_APP_3D_TERRAIN_DEF_HPP

#include "../../util/Vec2.hpp"
#include "../../util/Color.hpp"
#include "../../util/Trace.hpp"
#include "ResourceDef.hpp"

#include <irrlicht/irrlicht.h>

namespace engine
{
namespace app3D
{

class TerrainDef : public ResourceDef, public Tracked <TerrainDef>
{
public:
    TerrainDef();

    TerrainDef(const TerrainDef &) = delete;
    TerrainDef(TerrainDef &&) = default;

    TerrainDef &operator = (const TerrainDef &) = delete;
    TerrainDef &operator = (TerrainDef &&) = default;

    void expose(DataFile::Node &node) override;

    void dropIrrObjects() override;
    void reloadIrrObjects() override;

    bool isFlat() const;
    float getScale() const;
    float getSlopeDistortion() const;
    int getMaxLOD() const;
    int getSmoothFactor() const;
    bool usesGrass() const;
    const Color &getGrassColor() const;
    std::string getHeightMapPath() const;
    irr::video::ITexture &getTexture1() const;
    irr::video::ITexture &getTexture2() const;
    irr::video::ITexture &getTexture3() const;
    irr::video::ITexture &getSlopeTexture() const;
    irr::video::ITexture &getHeightMap() const;
    irr::video::ITexture &getNormalMap() const;
    irr::video::ITexture &getSplatMap() const;
    irr::video::IImage &getNormalMapImage() const;
    irr::video::IImage &getSplatMapImage() const;
    int getMostDominantGroundTextureIndex(const FloatVec2 &pos) const;

private:
    using base = ResourceDef;

    static const std::string k_terrainTexturesDirectory;
    static const std::string k_terrainTexturesExtension;

    std::string m_resourcePath;
    std::string m_texture1Path;
    std::string m_texture2Path;
    std::string m_texture3Path;
    std::string m_slopeTexturePath;
    std::string m_grassColorMapImagePath;
    std::string m_grassDensityMapImagePath;
    bool m_isFlat;
    float m_scale;
    float m_slopeDistortion;
    int m_maxLOD;
    int m_smoothFactor;
    bool m_useGrass;
    Color m_grassColor;
    irr::video::ITexture *m_texture1;
    irr::video::ITexture *m_texture2;
    irr::video::ITexture *m_texture3;
    irr::video::ITexture *m_slopeTexture;
    irr::video::ITexture *m_heightMap;
    irr::video::ITexture *m_normalMap;
    irr::video::ITexture *m_splatMap;
    irr::video::IImage *m_normalMapImage;
    irr::video::IImage *m_splatMapImage;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_TERRAIN_DEF_HPP
