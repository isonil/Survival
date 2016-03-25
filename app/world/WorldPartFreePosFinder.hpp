#ifndef APP_WORLD_PART_FREE_POS_FINDER_HPP
#define APP_WORLD_PART_FREE_POS_FINDER_HPP

#include "engine/util/Vec3.hpp"
#include "PlacementPredicates.hpp"
#include "WorldPart.hpp"

namespace engine { namespace app3D { class Terrain; class TerrainDef; } }

namespace app
{

class WorldPartTopographyInfo;

class WorldPartFreePosFinder
{
public:
    WorldPartFreePosFinder(const engine::app3D::Terrain &terrain, const std::shared_ptr <WorldPartTopographyInfo> &topography, const engine::FloatVec3 &entitiesInWorldPosOffset);

    std::experimental::optional <engine::FloatVec3> getRandomPosMatching(const PlacementPredicates &predicates);
    std::pair <engine::FloatVec3, bool> getPathFoundNextCheckpoint(const engine::FloatVec2 &from, const engine::FloatVec2 &to);
    void useFieldAt(const engine::FloatVec2 &pos);
    void setDirty();

private:
    struct Field
    {
        float height{};
        bool isUsed{};
        bool isSlopeWalkable{};
        engine::FloatVec3 pos;

        // A* or BFS
        float scoreF{}, scoreG{}, scoreH{};
        engine::IntVec2 cameFrom, goesTo;
        //bool visited{};

        // expirable bools
        int isInClosedSet{};
        int isInOpenSet{};
    };

    struct AStarComparator
    {
        bool operator () (int lhs, int rhs);

        WorldPartFreePosFinder &freePosFinder;
    };

    friend struct AStarComparator;

    void recalculateUsedFields();
    void createFieldsIndicesRandomShuffled();
    bool runPathFindingAlgorithmOnFields(const engine::IntVec2 &start, const engine::IntVec2 &end);
    void reconstructPathFindingAlgorithmPath(const engine::IntVec2 &start, engine::IntVec2 current);

    Field &getField(const engine::IntVec2 &tile);
    const Field &getField(const engine::IntVec2 &tile) const;
    bool isInBounds(const engine::IntVec2 &tile) const;
    bool isPassThroughAble(const Field &field) const;
    bool isPassThroughAble(const engine::IntVec2 &tile) const;
    engine::IntVec2 realPosToTile(const engine::FloatVec2 &pos) const;
    engine::IntVec2 realPosToTile(const engine::FloatVec3 &pos) const;
    engine::IntVec2 getClosestTileInBoundsToTile(const engine::IntVec2 &tile) const;

    static const float k_fieldSize;
    static const float k_maxWalkableSlope;
    static const int k_maxPathFindingAlgorithmIterations;

    const AStarComparator m_AStarComparator;
    const engine::FloatVec3 m_entitiesInWorldPosOffset;

    bool m_dirty;
    std::vector <int> m_fieldsIndicesRandomShuffled;
    std::vector <Field> m_fields;
    std::shared_ptr <WorldPartTopographyInfo> m_topography;
    std::shared_ptr <engine::app3D::TerrainDef> m_terrainDef;
    int m_size;
    int m_boolTrue; // for expirable bools

    // working vars, (so they don't reallocate memory each time they are used)
    std::vector <int> m_neighborNodesWorkingVar;
    std::set <int, AStarComparator> m_openSetWorkingVar;
};

} // namespace app

#endif // APP_WORLD_PART_FREE_POS_FINDER_HPP
