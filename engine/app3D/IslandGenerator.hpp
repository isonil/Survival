#ifndef ENGINE_APP_3D_ISLAND_GENERATOR_HPP
#define ENGINE_APP_3D_ISLAND_GENERATOR_HPP

#include "../util/Vec2.hpp"
#include "../ext/PerlinNoise.hpp"

#include <irrlicht.h>

#include <unordered_set>

namespace engine
{
namespace app3D
{

class Device;
class Island;

// experimental class, not used anywhere
class IslandGenerator
{
public:
    IslandGenerator();

    std::shared_ptr <Island> generateIsland(const std::weak_ptr <Device> &device);

private:
    struct Field
    {
        bool isIsland{};
        float height{};
        float bottomHeight{};
        float distanceToBorder{};
        bool topAndBottomConnection{};
        float wv_distToBorder[4];
    };

    Field &getField(int x, int y);
    float getHeightAt(const IntVec2 &point);
    IntVec2 pickRandomPoint();
    void floodFill(const IntVec2 &point, std::unordered_set <IntVec2> &visited);
    void generateFields();
    void scaleFields();
    void calculateFieldsDistanceToBorder();
    void calculateHeightAtEachField();
    void tryToMoveVerticesCloser(irr::video::S3DVertex &v1, irr::video::S3DVertex &v2);
    void generateVerticesAndIndices();

    static const int k_maxIslandSideLength;
    static const float k_minHeightForIsland;
    static const int k_minPointsPerIsland;
    static const int k_maxPointsPerIsland;
    static const float k_scaleFieldsFactor;
    static const float k_unitToUVScale;
    static const float k_unitToRealPosScale;

    int m_fieldsSideLength;

    PerlinNoise m_noise;
    PerlinNoise m_noise2;
    PerlinNoise m_noise3;
    PerlinNoise m_noise4;
    std::vector <Field> m_fields;

    std::vector <irr::video::S3DVertex> m_vertices;
    std::vector <int> m_indices;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_ISLAND_GENERATOR_HPP
