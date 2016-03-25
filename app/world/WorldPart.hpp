#ifndef APP_WORLD_PART_HPP
#define APP_WORLD_PART_HPP

#include "engine/ext/optional.hpp"
#include "engine/util/Trace.hpp"
#include "engine/util/DataFile.hpp"
#include "engine/util/Vec2.hpp"
#include "engine/util/Vec3.hpp"
#include "GroundType.hpp"

#include <memory>

namespace engine { namespace app3D { class Terrain; class Water; class RigidBody; } }

namespace app
{

class World;
class WorldPartDef;
class WorldPartTopographyInfo;
class WorldPartFreePosFinder;
class PlacementPredicates;

class WorldPart : public engine::DataFile::Saveable, public engine::Tracked <WorldPart>
{
public:
    void expose(engine::DataFile::Node &node) override;

    void makeItWaterWorldPart(const engine::IntVec2 &tilePosition);

    void generateEntities(World &world);
    const engine::IntVec2 &getTilePosition() const;
    float getHeight(const engine::FloatVec2 &pos) const;
    float getSlope(const engine::FloatVec2 &pos) const;
    GroundType getGroundType(const engine::FloatVec2 &pos) const;
    std::experimental::optional <engine::FloatVec3> getRandomPosMatching_worldPos(const PlacementPredicates &predicates);
    std::pair <engine::FloatVec3, bool> getPathFoundNextCheckpoint_worldPos(const engine::FloatVec2 &from, const engine::FloatVec2 &to);
    void setFreePosFinderDirty();
    void useFreePosFinderFieldAt(const engine::FloatVec2 &pos);
    WorldPartDef &getDef() const;

    static const float k_terrainSize;
    static const float k_waterHeight;

private:
    void addTerrainAndWater();

    static const float k_slopeGroundTypeSlopeThreshold;

    std::string m_worldPartDef_defName;
    engine::IntVec2 m_tilePosition;
    std::shared_ptr <WorldPartDef> m_worldPartDef;
    std::shared_ptr <engine::app3D::Terrain> m_terrain;
    std::shared_ptr <engine::app3D::Water> m_water;
    std::shared_ptr <engine::app3D::RigidBody> m_terrainRigidBody;
    std::shared_ptr <WorldPartTopographyInfo> m_topography;
    std::shared_ptr <WorldPartFreePosFinder> m_freePosFinder;
};

} // namespace app

#endif // APP_WORLD_PART_HPP
