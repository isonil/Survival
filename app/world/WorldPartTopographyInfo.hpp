#ifndef APP_WORLD_PART_TOPOGRAPHY_INFO_HPP
#define APP_WORLD_PART_TOPOGRAPHY_INFO_HPP

#include "engine/util/Vec2.hpp"
#include "engine/util/Vec3.hpp"

namespace engine { namespace app3D { class Terrain; } }

namespace app
{

class WorldPartTopographyInfo
{
public:
    WorldPartTopographyInfo(const engine::app3D::Terrain &terrain);

    float getSlope(engine::FloatVec2 pos);
    float getDistanceToWater(engine::FloatVec2 pos);

private:
    struct Field
    {
        engine::FloatVec3 pos;
        float height{};
        float slope{};
        float distanceToWater{};
    };

    void calculateSlope();
    void calculateDistanceToWater();

    void posToBounds(engine::FloatVec2 &pos) const;
    void calculateClosestFields(engine::FloatVec2 pos);

    Field &getField(int x, int y);
    const Field &getField(int x, int y) const;
    bool isInBounds(int x, int y) const;

    static const float k_fieldSize;

    std::vector <Field> m_fields;
    std::vector <Field> m_closestFields;
    int m_size;
};

} // namespace app

#endif // APP_WORLD_PART_TOPOGRAPHY_INFO_HPP

