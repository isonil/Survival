#include "TerrainDef.hpp"

#include "../Device.hpp"
#include "../managers/ResourcesManager.hpp"

namespace engine
{
namespace app3D
{

TerrainDef::TerrainDef()
    : m_resourcePath{"undefined"},
      m_isFlat{},
      m_scale{1.f},
      m_slopeDistortion{},
      m_maxLOD{4},
      m_smoothFactor{2},
      m_useGrass{},
      m_texture1{},
      m_texture2{},
      m_texture3{},
      m_slopeTexture{},
      m_heightMap{},
      m_normalMap{},
      m_splatMap{},
      m_normalMapImage{},
      m_splatMapImage{}
{
}

void TerrainDef::expose(DataFile::Node &node)
{
    base::expose(node);

    node.var(m_resourcePath, "resourcePath", "");
    node.var(m_texture1Path, "texture1Path");
    node.var(m_texture2Path, "texture2Path");
    node.var(m_texture3Path, "texture3Path");
    node.var(m_slopeTexturePath, "slopeTexturePath");
    node.var(m_isFlat, "isFlat", false);
    node.var(m_scale, "scale", 1.f);
    node.var(m_slopeDistortion, "slopeDistortion", {});
    node.var(m_maxLOD, "maxLOD", 4);
    node.var(m_smoothFactor, "smoothFactor", 2);
    node.var(m_useGrass, "useGrass");
    node.var(m_grassColor, "grassColor", Color::k_white);

    if(node.getActivityType() == DataFile::Activity::Type::Loading) {
        if(isFlat() && !m_resourcePath.empty()) {
            const auto &defName = getDefName();

            E_WARNING("Terrain \"%s\" is flat, so resourcePath does not affect it and can be omitted.",
                      defName.c_str());
        }

        if(m_scale <= 0.f)
            throw Exception{"Scale can't be <= 0."};

        if(m_slopeDistortion < 0.f)
            throw Exception{"Slope distortion can't be negative."};

        if(m_maxLOD <= 0)
            throw Exception{"Max LOD can't be <= 0."};

        if(m_smoothFactor < 0)
            throw Exception{"Smooth factor can't be negative."};
    }
}

void TerrainDef::dropIrrObjects()
{
    TRACK;
}

void TerrainDef::reloadIrrObjects()
{
    TRACK;

    auto &device = getDevice_slow();
    auto &rmg = device.getResourcesManager();

    if(!isFlat()) {
        m_heightMap = &rmg.loadIrrTexture(k_terrainTexturesDirectory + m_resourcePath + "/heightMap" + k_terrainTexturesExtension, true);
        m_normalMap = &rmg.loadIrrTexture(k_terrainTexturesDirectory + m_resourcePath + "/normalMap" + k_terrainTexturesExtension, true);
        m_splatMap = &rmg.loadIrrTexture(k_terrainTexturesDirectory + m_resourcePath + "/splatMap" + k_terrainTexturesExtension, true);

        auto &driver = *device.getIrrDevice().getVideoDriver();

        m_normalMapImage = driver.createImageFromData(
           m_normalMap->getColorFormat(),
           m_normalMap->getSize(),
           m_normalMap->lock(),
           false);

        m_normalMap->unlock();

        if(!m_normalMapImage)
            throw Exception{"Could not create normal map image."};

        m_splatMapImage = driver.createImageFromData(
           m_splatMap->getColorFormat(),
           m_splatMap->getSize(),
           m_splatMap->lock(),
           false);

        m_splatMap->unlock();

        if(!m_splatMapImage)
            throw Exception{"Could not create splat map image."};
    }

    m_texture1 = &rmg.loadIrrTexture(m_texture1Path, true);
    m_texture2 = &rmg.loadIrrTexture(m_texture2Path, true);
    m_texture3 = &rmg.loadIrrTexture(m_texture3Path, true);
    m_slopeTexture = &rmg.loadIrrTexture(m_slopeTexturePath, true);
}

bool TerrainDef::isFlat() const
{
    return m_isFlat;
}

float TerrainDef::getScale() const
{
    return m_scale;
}

float TerrainDef::getSlopeDistortion() const
{
    return m_slopeDistortion;
}

int TerrainDef::getMaxLOD() const
{
    return m_maxLOD;
}

int TerrainDef::getSmoothFactor() const
{
    return m_smoothFactor;
}

bool TerrainDef::usesGrass() const
{
    return m_useGrass;
}

const Color &TerrainDef::getGrassColor() const
{
    return m_grassColor;
}

std::string TerrainDef::getHeightMapPath() const
{
    if(isFlat())
        throw Exception{"Tried to get height map path from flat terrain \"" + getDefName() + "\"."};

    if(!m_heightMap)
        throw Exception{"Height map is nullptr."};

    return m_heightMap->getName().getPath().c_str();
}

irr::video::ITexture &TerrainDef::getTexture1() const
{
    TRACK;

    if(!m_texture1)
        throw Exception{"Texture 1 is nullptr."};

    return *m_texture1;
}

irr::video::ITexture &TerrainDef::getTexture2() const
{
    TRACK;

    if(!m_texture2)
        throw Exception{"Texture 2 is nullptr."};

    return *m_texture2;
}

irr::video::ITexture &TerrainDef::getTexture3() const
{
    TRACK;

    if(!m_texture3)
        throw Exception{"Texture 3 is nullptr."};

    return *m_texture3;
}

irr::video::ITexture &TerrainDef::getSlopeTexture() const
{
    TRACK;

    if(!m_slopeTexture)
        throw Exception{"Slope texture is nullptr."};

    return *m_slopeTexture;
}

irr::video::ITexture &TerrainDef::getHeightMap() const
{
    TRACK;

    if(!m_heightMap)
        throw Exception{"Height map is nullptr."};

    return *m_heightMap;
}

irr::video::ITexture &TerrainDef::getNormalMap() const
{
    TRACK;

    if(!m_normalMap)
        throw Exception{"Normal map is nullptr."};

    return *m_normalMap;
}

irr::video::ITexture &TerrainDef::getSplatMap() const
{
    TRACK;

    if(!m_splatMap)
        throw Exception{"Splat map is nullptr."};

    return *m_splatMap;
}

irr::video::IImage &TerrainDef::getNormalMapImage() const
{
    TRACK;

    if(!m_normalMapImage)
        throw Exception{"Normal map image is nullptr."};

    return *m_normalMapImage;
}

irr::video::IImage &TerrainDef::getSplatMapImage() const
{
    TRACK;

    if(!m_splatMapImage)
        throw Exception{"Splat map image is nullptr."};

    return *m_splatMapImage;
}

int TerrainDef::getMostDominantGroundTextureIndex(const FloatVec2 &pos) const
{
    if(!m_splatMapImage) // this means that it's a flat terrain (flat terrain acts as if it had a full red splat map)
        return 0;

    const auto &dimension = m_splatMapImage->getDimension();

    if(!dimension.Width || !dimension.Height)
        return 0;

    auto x = static_cast <int> (dimension.Width - pos.x / m_scale * dimension.Width);
    auto y = static_cast <int> (pos.y / m_scale * dimension.Height);

    x = Math::clamp(x, 0, dimension.Width - 1);
    y = Math::clamp(y, 0, dimension.Height - 1);

    const auto &pixel = m_splatMapImage->getPixel(x, y);

    auto maxColor = std::max({pixel.getRed(), pixel.getGreen(), pixel.getBlue()});

    if(maxColor == pixel.getRed())
        return 0;
    else if(maxColor == pixel.getGreen())
        return 1;
    else {
        E_DASSERT(maxColor == pixel.getBlue(), "Something wrong with == comparison.");
        return 2;
    }
}

const std::string TerrainDef::k_terrainTexturesDirectory = "terrain/";
const std::string TerrainDef::k_terrainTexturesExtension = ".bmp";

} // namespace app3D
} // namespace engine
