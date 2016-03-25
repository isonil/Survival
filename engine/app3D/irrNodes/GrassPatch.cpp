#include "GrassPatch.hpp"

#include "../../util/Exception.hpp"
#include "../../util/WindGenerator.hpp"
#include "../IrrlichtConversions.hpp"

namespace engine
{
namespace app3D
{
namespace irrNodes
{

GrassPatch::GrassPatch(irr::scene::ISceneManager &sceneManager, irr::scene::ITerrainSceneNode &terrain, irr::scene::ISceneNode &parent,
                       const FloatVec2 &pos, const FloatVec2 &terrainSize,
                       irr::video::IImage &normalMapImage, irr::video::IImage &splatMapImage,
                       const Color &grassColor, const IntVec2 &texturesInTextureCount, const std::shared_ptr <WindGenerator> &windGenerator)
    :   ISceneNode(&parent, &sceneManager, -1),
        m_windGenerator{windGenerator},
        m_pos{pos.x, 0.f, pos.y},
        m_terrainSize{terrainSize},
        m_grassColor{grassColor},
        m_terrain{terrain},
        m_normalMapImage{normalMapImage},
        m_splatMapImage{splatMapImage},
        m_drawDistSq{k_defaultDrawDist * k_defaultDrawDist},
        m_windGridRes{},
        m_lastWindChangeTime{},
        m_lastDrawCount{},
        m_timeBetweenAnimationFrames{k_defaultTimeBetweenAnimationFrames},
        m_redrawNextLoop{true}
{
    if(!m_windGenerator)
        throw Exception{"Wind generator is nullptr."};

    if(texturesInTextureCount.x <= 0 || texturesInTextureCount.y <= 0)
        throw Exception{"There must be at least one texture."};

    setPosition({pos.x, 0.f, pos.y});
    updateAbsolutePosition();

    setTexturesInTexture(texturesInTextureCount);
    setWindRes(k_windRes);

    Random::Generator randGenerator;
    randGenerator.seed(100 * pos.x + pos.y);

    int particleCount{k_grassQuadsCount};
    m_particles.resize(particleCount);

    m_boundingBox.reset(0, 0, 0);

    float prevX{};
    float prevZ{};
    float prevRot{};
    bool hasPrev{};

    const auto &absPos = getAbsolutePosition();

    E_DASSERT(m_texturesInTextureCount.Width, "No textures.");
    E_DASSERT(m_texturesInTextureCount.Height, "No textures.");

    for(int i = 0; i < particleCount; ++i) {
        float x{}, z{};

        if(i % 2 && hasPrev) {
            x = prevX;
            z = prevZ;
        }
        else {
            x = Random::rangeInclusive(-k_grassPatchSize / 2.f, k_grassPatchSize / 2.f, randGenerator);
            z = Random::rangeInclusive(-k_grassPatchSize / 2.f, k_grassPatchSize / 2.f, randGenerator);
        }

        m_particles[i].pos.X = x;
        m_particles[i].pos.Z = z;
        m_particles[i].sprite.Width = Random::rangeExclusive(0, m_texturesInTextureCount.Width, randGenerator);
        m_particles[i].sprite.Height = Random::rangeExclusive(0, m_texturesInTextureCount.Height, randGenerator);

        float percentX{(pos.x + m_particles[i].pos.X) / m_terrainSize.x};
        float percentY{(pos.y + m_particles[i].pos.Z) / m_terrainSize.y};
        auto normalMapX = static_cast <int> (m_normalMapImage.getDimension().Width - percentX * m_normalMapImage.getDimension().Width);
        auto splatMapX = static_cast <int> (m_splatMapImage.getDimension().Width - percentX * m_splatMapImage.getDimension().Width);

        if(percentX <= 0.f || percentX >= 1.f || percentY <= 0.f || percentY >= 1.f ||
           normalMapX >= static_cast <int> (m_normalMapImage.getDimension().Width) ||
           splatMapX >= static_cast <int> (m_splatMapImage.getDimension().Width)) {
            --particleCount;
            --i;
            m_particles.resize(particleCount);
            continue;
        }

        auto normalMapZ = static_cast <int> (percentY * m_normalMapImage.getDimension().Height);
        auto splatMapZ = static_cast <int> (percentY * m_splatMapImage.getDimension().Height);

        E_DASSERT(normalMapX >= 0 && normalMapX < static_cast <int> (m_normalMapImage.getDimension().Width) &&
                  normalMapZ >= 0 && normalMapZ < static_cast <int> (m_normalMapImage.getDimension().Height), "Pixel coords out of bounds.");

        E_DASSERT(splatMapX >= 0 && splatMapX < static_cast <int> (m_splatMapImage.getDimension().Width) &&
                  splatMapZ >= 0 && splatMapZ < static_cast <int> (m_splatMapImage.getDimension().Height), "Pixel coords out of bounds.");

        auto normalMapCol = m_normalMapImage.getPixel(normalMapX, normalMapZ);
        auto splatMapCol = m_splatMapImage.getPixel(splatMapX, splatMapZ);

        if(static_cast <int> (splatMapCol.getGreen()) < k_splatMapGreenColorThreshold ||
           static_cast <int> (normalMapCol.getBlue()) < k_normalMapUpVectorThreshold) {
            --particleCount;
            --i;
            m_particles.resize(particleCount);
            continue;
        }

        float height{m_terrain.getHeight(absPos.X + m_particles[i].pos.X, absPos.Z + m_particles[i].pos.Z)};
        irr::core::dimension2df size{1.2f, 1.1f + Random::rangeInclusive(0.f, 0.2f, randGenerator)};

        m_particles[i].height = size.Height;
        m_particles[i].flex = size.Height * k_heightToFlexFactor;
        m_particles[i].pos.Y = height + (size.Height * 0.5f);
        m_particles[i].color = IrrlichtConversions::toColor(m_grassColor);

        float rotation{};

        if(i % 2 && hasPrev)
            rotation = prevRot + 90.f + Random::rangeInclusive(-30.f, 30.f, randGenerator);
        else
            rotation = Random::rangeInclusive(0.f, 360.f, randGenerator);

        irr::core::matrix4 mat;
        mat.setRotationDegrees({0.f, rotation, 0.f});

        irr::core::vector3df dimensions{0.5f * size.Width, -0.5f * size.Height, 0.f};
        mat.rotateVect(dimensions);

        if(!i)
            m_boundingBox.reset(m_particles[i].pos);
        else
            m_boundingBox.addInternalPoint(m_particles[i].pos);

        m_particles[i].points[0] = m_particles[i].pos + irr::core::vector3df{dimensions.X, dimensions.Y, dimensions.Z};
        m_particles[i].points[1] = m_particles[i].pos + irr::core::vector3df{dimensions.X, -dimensions.Y, dimensions.Z};
        m_particles[i].points[2] = m_particles[i].pos - irr::core::vector3df{dimensions.X, dimensions.Y, dimensions.Z};
        m_particles[i].points[3] = m_particles[i].pos - irr::core::vector3df{dimensions.X, -dimensions.Y, dimensions.Z};

        prevX = x;
        prevZ = z;
        prevRot = rotation;
        hasPrev = true;
    }

    allocateBuffers();
}

void GrassPatch::setTexturesInTexture(const IntVec2 &count)
{
    m_texturesInTextureCount = {count.x, count.y};

    irr::core::dimension2df imageSize;

    imageSize.Width = 1.f / m_texturesInTextureCount.Width;
    imageSize.Height = 1.f / m_texturesInTextureCount.Height;

    m_vertex1TexCoords.resize(m_texturesInTextureCount.Width * m_texturesInTextureCount.Height);
    m_vertex2TexCoords.resize(m_texturesInTextureCount.Width * m_texturesInTextureCount.Height);
    m_vertex3TexCoords.resize(m_texturesInTextureCount.Width * m_texturesInTextureCount.Height);
    m_vertex4TexCoords.resize(m_texturesInTextureCount.Width * m_texturesInTextureCount.Height);

    for(int x = 0; x < m_texturesInTextureCount.Width; ++x) {
        for(int y = 0; y < m_texturesInTextureCount.Height; ++y) {
            m_vertex1TexCoords[m_texturesInTextureCount.Width * y + x] = imageSize.Width * x;
            m_vertex2TexCoords[m_texturesInTextureCount.Width * y + x] = imageSize.Height * (y + 1);
            m_vertex3TexCoords[m_texturesInTextureCount.Width * y + x] = imageSize.Height * y;
            m_vertex4TexCoords[m_texturesInTextureCount.Width * y + x] = imageSize.Width * (x + 1);
        }
    }
}

irr::video::SMaterial &GrassPatch::getMaterial(irr::u32 i)
{
    return m_material;
}

irr::u32 GrassPatch::getMaterialCount() const
{
    return 1;
}

const irr::core::aabbox3d <irr::f32> &GrassPatch::getBoundingBox() const
{
    return m_boundingBox;
}

void GrassPatch::OnRegisterSceneNode()
{
    if(IsVisible) {
        if(!m_particles.empty()) {
            const auto &camPos = SceneManager->getActiveCamera()->getPosition();

            if((m_boundingBox.getCenter() + getAbsolutePosition()).getDistanceFrom(camPos) < std::sqrt(m_drawDistSq) + (m_boundingBox.getExtent() / 2.f).getLength())
                SceneManager->registerNodeForRendering(this, irr::scene::ESNRP_SOLID);
        }

        ISceneNode::OnRegisterSceneNode();
    }
}

void GrassPatch::OnAnimate(irr::u32 timeMs)
{
    if(IsVisible) {
        if(m_lastWindChangeTime + m_timeBetweenAnimationFrames < timeMs) {
            m_lastWindChangeTime = timeMs;
            m_redrawNextLoop = true;

            float dist{(m_boundingBox.getCenter() + getAbsolutePosition()).getDistanceFrom(SceneManager->getActiveCamera()->getPosition())};

            if(m_windGenerator && dist < std::sqrt(m_drawDistSq) + (m_boundingBox.getExtent() / 2.f).getLength()) {
                for(int x = 0; x < m_windGridRes + 1; ++x) {
                    for(int z = 0; z < m_windGridRes + 1; ++z) {
                        FloatVec3 p{m_pos.X + x * (k_grassPatchSize / m_windGridRes) - k_grassPatchSize / 2.f,
                                    0.f,
                                    m_pos.Z + z * (k_grassPatchSize / m_windGridRes) - k_grassPatchSize / 2.f};

                        const auto &wind = m_windGenerator->getWind(p, timeMs);

                        int index{x * (m_windGridRes + 1) + z};

                        E_DASSERT(index < static_cast <int> (m_windGrid.size()), "Index out of bounds.");

                        m_windGrid[index].set(wind.x, wind.y);
                    }
                }
            }
        }
        else
            m_redrawNextLoop = false;

        ISceneNode::OnAnimate(timeMs);
    }
}

void GrassPatch::render()
{
    auto *driver = SceneManager->getVideoDriver();
    auto *camera = SceneManager->getActiveCamera();

    if(!camera || !driver)
        return;

    if(!m_redrawNextLoop) {
        E_DASSERT(m_vertices.size() >= m_lastDrawCount * 4, "Invalid vertices count.");
        E_DASSERT(m_indices.size() >= m_lastDrawCount * 6, "Invalid indices count.");

        driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);
        driver->setMaterial(m_material);
        driver->drawIndexedTriangleList(m_vertices.data(), m_lastDrawCount * 4,
                                        m_indices.data(), m_lastDrawCount * 2);
	}
    else {
        const auto &camPos = camera->getAbsolutePosition();
        const auto &camBox = camera->getViewFrustum()->getBoundingBox();
        const auto &pos = getAbsolutePosition();

        int drawCount{};

        for(size_t i = 0; i < m_particles.size(); ++i) {
            int index{drawCount * 4};
            auto &thisParticle = m_particles[i];
            const auto &particleAbsPos = thisParticle.pos + pos;

            if(!camBox.isPointInside(particleAbsPos))
                continue;

            float dist{camPos.getDistanceFromSQ(particleAbsPos)};
            float offsetY{};

            if(dist > m_drawDistSq)
                continue;
            else if(dist > m_drawDistSq / 2.f)
                offsetY = -(1.f - (m_drawDistSq - dist) / (m_drawDistSq / 2.f)) * thisParticle.height;

            float gridSize{k_grassPatchSize / m_windGridRes};

            float xGridFloat{(thisParticle.pos.X + k_grassPatchSize / 2.f) / gridSize};
            float zGridFloat{(thisParticle.pos.Z + k_grassPatchSize / 2.f) / gridSize};

            if(thisParticle.pos.X + k_grassPatchSize / 2.f > k_grassPatchSize)
                std::cout << i << " outside by: " << thisParticle.pos.X + k_grassPatchSize / 2.f - k_grassPatchSize << std::endl;

            if(thisParticle.pos.Z + k_grassPatchSize / 2.f > k_grassPatchSize)
                std::cout << i << " outside by: " << thisParticle.pos.Z + k_grassPatchSize / 2.f - k_grassPatchSize << std::endl;

            if(xGridFloat < 0.f)
                xGridFloat = 0.f;

            if(zGridFloat < 0.f)
                zGridFloat = 0.f;

            auto xGrid = static_cast <int> (xGridFloat);
            auto zGrid = static_cast <int> (zGridFloat);

            float xNext{std::fmod(xGridFloat, 1.f)}; // it's [0;1] distance to this int cell
            float zNext{std::fmod(zGridFloat, 1.f)}; // it's [0;1] distance to this int cell

            int topLeft{xGrid * (m_windGridRes + 1) + zGrid};
            int topRight{xGrid * (m_windGridRes + 1) + zGrid + 1};
            int botLeft{(xGrid + 1) * (m_windGridRes + 1) + zGrid};
            int botRight{(xGrid + 1) * (m_windGridRes + 1) + zGrid + 1};

            E_DASSERT(topLeft >= 0 && topLeft < static_cast <int> (m_windGrid.size()), "Index out of bounds.");
            E_DASSERT(topRight >= 0 && topRight < static_cast <int> (m_windGrid.size()), "Index out of bounds.");
            E_DASSERT(botLeft >= 0 && botLeft < static_cast <int> (m_windGrid.size()), "Index out of bounds.");
            E_DASSERT(botRight >= 0 && botRight < static_cast <int> (m_windGrid.size()), "Index out of bounds.");

            const auto &wind1 = m_windGrid[topLeft];
            const auto &wind2 = m_windGrid[botLeft];
            const auto &wind3 = m_windGrid[topRight];
            const auto &wind4 = m_windGrid[botRight];

            const auto &wind2D = (wind1 * (1.f - xNext) * (1.f - zNext) +
                                  wind2 * xNext * (1.f - zNext) +
                                  wind3 * (1.f - xNext) * zNext +
                                  wind4 * xNext * zNext) * thisParticle.flex;

            irr::core::vector3df windFin{wind2D.X, 0.f, wind2D.Y};

            irr::video::SColor botCol{thisParticle.color.getAlpha(),
                                      static_cast <irr::u32> (thisParticle.color.getRed() * k_colorMultiplierForBottomVertices),
                                      static_cast <irr::u32> (thisParticle.color.getGreen() * k_colorMultiplierForBottomVertices),
                                      static_cast <irr::u32> (thisParticle.color.getBlue() * k_colorMultiplierForBottomVertices)};

            m_vertices[0 + index].Pos = thisParticle.points[0];
            m_vertices[0 + index].Pos.Y += offsetY;
            m_vertices[0 + index].Color = botCol;

            m_vertices[1 + index].Pos = thisParticle.points[1] + windFin;
            m_vertices[1 + index].Pos.Y += offsetY;
            m_vertices[1 + index].Color = thisParticle.color;

            m_vertices[2 + index].Pos = thisParticle.points[2] + windFin;
            m_vertices[2 + index].Pos.Y += offsetY;
            m_vertices[2 + index].Color = thisParticle.color;

            m_vertices[3 + index].Pos = thisParticle.points[3];
            m_vertices[3 + index].Pos.Y += offsetY;
            m_vertices[3 + index].Color = botCol;

            auto texCoordsArrPos = (m_texturesInTextureCount.Width * thisParticle.sprite.Height) + thisParticle.sprite.Width;

            m_vertices[0 + index].TCoords.set(m_vertex1TexCoords[texCoordsArrPos], m_vertex2TexCoords[texCoordsArrPos]);
            m_vertices[1 + index].TCoords.set(m_vertex1TexCoords[texCoordsArrPos], m_vertex3TexCoords[texCoordsArrPos]);
            m_vertices[2 + index].TCoords.set(m_vertex4TexCoords[texCoordsArrPos], m_vertex3TexCoords[texCoordsArrPos]);
            m_vertices[3 + index].TCoords.set(m_vertex4TexCoords[texCoordsArrPos], m_vertex2TexCoords[texCoordsArrPos]);

            ++drawCount;
        }

        E_DASSERT(static_cast <int> (m_vertices.size()) >= drawCount * 4, "Invalid vertices count.");
        E_DASSERT(static_cast <int> (m_indices.size()) >= drawCount * 6, "Invalid indices count.");

        driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);
        driver->setMaterial(m_material);
        driver->drawIndexedTriangleList(m_vertices.data(), drawCount * 4,
                                        m_indices.data(), drawCount * 2);

        m_lastDrawCount = drawCount;
	}
}

const float GrassPatch::k_grassPatchSize{50.f};

void GrassPatch::setWindRes(int res)
{
    m_windGridRes = res < 2 ? 2 : res;
    m_windGrid.resize((m_windGridRes + 1) * (m_windGridRes + 1));
}

void GrassPatch::allocateBuffers()
{
    m_vertices.resize(m_particles.size() * 4);

    for(auto &elem : m_vertices) {
        elem.Normal.set(0.f, 1.f, 0.f);
    }

    m_indices.resize(m_particles.size() * 6);

    int vertexIndex{};

    for(size_t i = 0; i < m_indices.size(); i += 6) {
        m_indices[0 + i] = 0 + vertexIndex;
        m_indices[1 + i] = 1 + vertexIndex;
        m_indices[2 + i] = 2 + vertexIndex;
        m_indices[3 + i] = 2 + vertexIndex;
        m_indices[4 + i] = 3 + vertexIndex;
        m_indices[5 + i] = 0 + vertexIndex;

        vertexIndex += 4;
    }
}

const float GrassPatch::k_defaultDrawDist{100.f};
const irr::u32 GrassPatch::k_defaultTimeBetweenAnimationFrames{60u};
const int GrassPatch::k_grassQuadsCount{6000};
const int GrassPatch::k_splatMapGreenColorThreshold{160};
const int GrassPatch::k_normalMapUpVectorThreshold{236};
const int GrassPatch::k_windRes{20};
const float GrassPatch::k_heightToFlexFactor{1.f / 120.f};
const float GrassPatch::k_colorMultiplierForBottomVertices{0.8f};

} // namespace irrNodes
} // namespace app3D
} // namespace engine
