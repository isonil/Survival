#include "WorldPartFreePosFinder.hpp"

#include "engine/app3D/sceneNodes/Terrain.hpp"
#include "engine/app3D/defs/TerrainDef.hpp"
#include "../entities/Entity.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "World.hpp"
#include "WorldPartTopographyInfo.hpp"

#include <queue>

namespace app
{

WorldPartFreePosFinder::WorldPartFreePosFinder(const engine::app3D::Terrain &terrain, const std::shared_ptr <WorldPartTopographyInfo> &topography, const engine::FloatVec3 &entitiesInWorldPosOffset)
    : m_AStarComparator{*this},
      m_entitiesInWorldPosOffset{entitiesInWorldPosOffset},
      m_dirty{},
      m_topography{topography},
      m_terrainDef{terrain.getDefPtr()},
      m_size{},
      m_boolTrue{1},
      m_neighborNodesWorkingVar(4, 0),
      m_openSetWorkingVar(m_AStarComparator)
{
    TRACK;

    if(!m_topography)
        throw engine::Exception{"Topography is nullptr."};

    auto size = static_cast <int> (terrain.getDef().getScale() / k_fieldSize);

    if(!size)
        throw engine::Exception{"Terrain scale can't be 0."};

    m_size = size;
    m_fields.reserve(m_size * m_size);

    for(int y = 0; y < size; ++y) {
        for(int x = 0; x < size; ++x) {
            engine::FloatVec2 pos{x * k_fieldSize + k_fieldSize / 2.f, y * k_fieldSize + k_fieldSize / 2.f};

            auto height = terrain.getHeight(pos);
            m_fields.emplace_back();
            m_fields.back().height = height;
            m_fields.back().pos = {pos.x, height, pos.y};
            m_fields.back().isSlopeWalkable = m_topography->getSlope({m_fields.back().pos.x, m_fields.back().pos.z}) <= k_maxWalkableSlope;
        }
    }

    createFieldsIndicesRandomShuffled();
}

std::experimental::optional <engine::FloatVec3> WorldPartFreePosFinder::getRandomPosMatching(const PlacementPredicates &predicates)
{
    TRACK;

    if(m_dirty)
        recalculateUsedFields();

    // TODO FIXME: assuming that entity takes exactly 1 tile

    for(size_t i = 0; i < m_fieldsIndicesRandomShuffled.size(); ++i) {
        int index{m_fieldsIndicesRandomShuffled[i]};

        E_DASSERT(index >= 0 && static_cast <size_t> (index) < m_fields.size(), "Index out of bounds.");

        auto &field = m_fields[index];

        if(field.isUsed)
            continue;

        if(predicates.getOnlyAboveWaterLevel() && field.height <= WorldPart::k_waterHeight)
            continue;

        if(predicates.getOnlyBelowWaterLevel() && field.height >= WorldPart::k_waterHeight)
            continue;

        E_DASSERT(m_topography, "Topography is nullptr.");

        if(!predicates.getSlopeRange().isInRange(m_topography->getSlope({field.pos.x, field.pos.z})))
            continue;

        if(!predicates.isAllowedGround1() || !predicates.isAllowedGround2() || !predicates.isAllowedGround3()) {
            E_DASSERT(m_terrainDef, "Terrain def is nullptr.");

            int groundType{m_terrainDef->getMostDominantGroundTextureIndex({field.pos.x, field.pos.z})};

            if(groundType == 0 && !predicates.isAllowedGround1())
                continue;

            if(groundType == 1 && !predicates.isAllowedGround2())
                continue;

            if(groundType == 2 && !predicates.isAllowedGround3())
                continue;
        }

        int randomIndex{engine::Random::rangeExclusive(0, m_fieldsIndicesRandomShuffled.size())};
        std::swap(m_fieldsIndicesRandomShuffled[i], m_fieldsIndicesRandomShuffled[randomIndex]);

        return field.pos;
    }

    return std::experimental::optional <engine::FloatVec3> {};
}

std::pair <engine::FloatVec3, bool> WorldPartFreePosFinder::getPathFoundNextCheckpoint(const engine::FloatVec2 &from, const engine::FloatVec2 &to)
{
    TRACK;

    if(m_dirty)
        recalculateUsedFields();

    // this method returns a pair: position and a bool indicating whether there is a
    // clear way to returned position. If this bool is false, then it means that this
    // method used 'best effort' method to get to the target and does not guarantee
    // that it will be possible to reach returned position nor that it will be optimal.

    // note that toTile can be out of bounds (in a different WorldPart), so we will try
    // to reach toClosest in our path finding algorithm
    const auto &fromTile = realPosToTile(from);
    const auto &toTile = realPosToTile(to);
    const auto &toClosest = getClosestTileInBoundsToTile(toTile);

    bool goStraightToTarget{};
    bool clearWay{};

    // if fromTile is in another WorldPart, then the only thing we can do is to try to get to the target directly
    if(!isInBounds(fromTile))
        goStraightToTarget = true;
    else if(std::abs(fromTile.x - toClosest.x) <= 1 &&
            std::abs(fromTile.y - toClosest.y) <= 1) {
        // if we reached toClosest, then just go directly to the target.
        // Note that if toTile was out of bounds, then if we'll go directly
        // to the target we'll switch WorldParts a few frames later

        goStraightToTarget = true;
        clearWay = true;
    }

    if(!goStraightToTarget) {
        // now we run path finding algorithm (note that we try to reach toClosest, and not toTile)

        bool found{runPathFindingAlgorithmOnFields(fromTile, toClosest)};

        if(found) {
            // we start looking for a checkpoint from the next tile after fromTile,
            // note that fromTile != toClosest (we checked it before) so we can safely
            // start from the next tile after fromTile

            auto currentTile = getField(fromTile).goesTo;

            int iterationsGuard{};

            while(currentTile != toClosest) {
                ++iterationsGuard;
                E_DASSERT(iterationsGuard < 1000000, "Probably infinite loop.");

                // we search for first tile that touches any non-walkable tile,
                // and then we'll go directly to it (it will be our 'checkpoint')
                if(!isPassThroughAble(currentTile.movedX(-1)) ||
                   !isPassThroughAble(currentTile.movedX(1)) ||
                   !isPassThroughAble(currentTile.movedY(-1)) ||
                   !isPassThroughAble(currentTile.movedY(1)))
                    break;

                currentTile = getField(currentTile).goesTo;
            }

            if(currentTile == toTile) {
                // if checkpoint tile is the same as destination tile, then just go directly to the target
                // (so we won't try to reach tile's center, but go directly to the target)

                goStraightToTarget = true;
                clearWay = true;
            }
            else {
                auto &checkpointTileField = getField(currentTile);
                return std::make_pair(checkpointTileField.pos, true);
            }
        }
        else
            goStraightToTarget = true; // best effort
    }

    if(goStraightToTarget) {
        float height{Global::getCore().getWorld().getHeight(to)};
        engine::FloatVec3 ret{to.x, height, to.y};
        return std::make_pair(ret, clearWay);
    }

    throw engine::Exception{"Unreachable code."};
}

void WorldPartFreePosFinder::useFieldAt(const engine::FloatVec2 &pos)
{
    const auto &tile = realPosToTile(pos);

    if(isInBounds(tile))
        getField(tile).isUsed = true;
}

void WorldPartFreePosFinder::setDirty()
{
    m_dirty = true;
}

bool WorldPartFreePosFinder::AStarComparator::operator () (int lhs, int rhs)
{
    // std::tie has too big overhead in debug mode
    //return std::tie(this->m_fields[lhs].scoreF, lhs) < std::tie(this->m_fields[rhs].scoreF, rhs);

    if(freePosFinder.m_fields[lhs].scoreF == freePosFinder.m_fields[rhs].scoreF)
        return lhs < rhs;
    else
        return freePosFinder.m_fields[lhs].scoreF < freePosFinder.m_fields[rhs].scoreF;
}

void WorldPartFreePosFinder::recalculateUsedFields()
{
    TRACK;

    for(auto &field : m_fields) {
        field.isUsed = false;
    }

    const auto &world = Global::getCore().getWorld();

    world.forEachEntity([this](auto &entity) {
        if(entity.blocksWorldPartFreePosFinderField()) {
            const auto &tile = this->realPosToTile(entity.getInWorldPosition() - this->m_entitiesInWorldPosOffset);

            // TODO FIXME: assuming that entity takes exactly 1 tile

            if(this->isInBounds(tile))
                this->getField(tile).isUsed = true;
        }
    });

    m_dirty = false;
}

void WorldPartFreePosFinder::createFieldsIndicesRandomShuffled()
{
    TRACK;

    m_fieldsIndicesRandomShuffled.reserve(m_fields.size());

    for(size_t i = 0; i < m_fields.size(); ++i) {
        m_fieldsIndicesRandomShuffled.emplace_back(i);
    }

    // TODO: depr?
    std::random_shuffle(m_fieldsIndicesRandomShuffled.begin(), m_fieldsIndicesRandomShuffled.end(), [](int to) {
        return engine::Random::rangeExclusive(0, to);
    });
}

/* A lot of work and testing has been put in making this algorithm.
 * Already tested candidates include:
 *  - Simple BFS (flood fill) using std::queue of IntVec2 - huge overhead even if end node is in straight line from start node. Too many nodes had to be checked before reaching target.
 *  - Simple BFS (flood fill) using std::deque of ints (exact field index) - faster than previous BFS, but still too slow. Too many nodes had to be checked before reaching target.
 *  - A* with std::set of IntVec2 for openset and closedset - adding/removing elements from std::set was the bottleneck. Too slow.
 *  - A* with heap in std::vector for openset (std::make_heap, std::push_heap, std::pop_heap); closedset implemented as O(1) expirable bools in each node - rebuilding whole heap (std::make_heap) when having to update F score for a node in the middle of the heap was the bottleneck.
 *    (if not for having to rebuild whole heap in O(n) it would be the fastest method probably).
 *  - Optimized A* with std::set of ints (exact field index) for openset; O(1) expirable bool for is-in-openset and is-in-closedset operations - this is the fastest method, and the one that is used currently. More info can be found in comments in the code.
 */
bool WorldPartFreePosFinder::runPathFindingAlgorithmOnFields(const engine::IntVec2 &start, const engine::IntVec2 &end)
{
    TRACK;

    // A* implementation

    E_DASSERT(isInBounds(start), "Start is out of bounds.");
    E_DASSERT(isInBounds(end), "End is out of bounds.");
    E_DASSERT(m_size, "Size is 0.");
    E_DASSERT(m_neighborNodesWorkingVar.size() == 4, "Neighbor nodes working var size is not 4.");

    if(!isPassThroughAble(end)) // early-out
        return false;

    ++m_boolTrue; // expirable bool current true value

    if(m_boolTrue == std::numeric_limits <int>::max()) {
        // all expirable bools should be set to 0 here,
        // otherwise collisions can occur (very rare case though)

        for(auto &elem : m_fields) {
            elem.isInClosedSet = 0;
            elem.isInOpenSet = 0;
        }

        m_boolTrue = 1;
    }

    int startNode{start.y * m_size + start.x};
    int endNode{end.y * m_size + end.x};

    m_fields[startNode].scoreF = 0.f;
    m_fields[startNode].scoreG = 0.f;
    m_fields[startNode].scoreH = 0.f;

    m_openSetWorkingVar.clear();
    m_openSetWorkingVar.insert(startNode);

    m_fields[startNode].isInOpenSet = m_boolTrue;

    int iterations{};

    while(!m_openSetWorkingVar.empty()) {
        ++iterations;

        if(iterations == k_maxPathFindingAlgorithmIterations) // there is an iterations limit
            return false;

        // get node with lowest F score and remove it from the set
        auto node = *m_openSetWorkingVar.begin();
        m_openSetWorkingVar.erase(m_openSetWorkingVar.begin());

        // change expirable bool to false
        m_fields[node].isInOpenSet = 0;

        // did we reach our goal?
        if(node == endNode) {
            reconstructPathFindingAlgorithmPath(start, end);
            return true;
        }

        // change expirable bool to true
        m_fields[node].isInClosedSet = m_boolTrue;

        // calculate neighbor nodes

        size_t neighborsCount{};

        if(node % m_size && isPassThroughAble(m_fields[node - 1])) { // left node
            m_neighborNodesWorkingVar[neighborsCount] = node - 1;
            ++neighborsCount;
        }

        if(node % m_size != m_size - 1 && isPassThroughAble(m_fields[node + 1])) { // right node
            m_neighborNodesWorkingVar[neighborsCount] = node + 1;
            ++neighborsCount;
        }

        if(node >= m_size && isPassThroughAble(m_fields[node - m_size])) { // up node
            m_neighborNodesWorkingVar[neighborsCount] = node - m_size;
            ++neighborsCount;
        }

        if(node + m_size < m_size * m_size && isPassThroughAble(m_fields[node + m_size])) { // down node
            m_neighborNodesWorkingVar[neighborsCount] = node + m_size;
            ++neighborsCount;
        }

        float nodeScoreG{m_fields[node].scoreG};

        for(size_t i = 0; i < neighborsCount; ++i) {
            const auto &node2 = m_neighborNodesWorkingVar[i];

            // skip if already in closed set
            if(m_fields[node2].isInClosedSet == m_boolTrue)
                continue;

            float tentativeScoreG{nodeScoreG + 1.f}; // distance between tiles is always 1 (not k_fieldSize)

            // if not in open set, add it
            if(m_fields[node2].isInOpenSet != m_boolTrue) {
                float distSq{engine::IntVec2{node2 % m_size, node2 / m_size}.getDistanceSq(end)};

                auto &node2Field = m_fields[node2];

                // we use dist^4 as our heuristic so our algorithm will try to go directly at the target direction (good for open fields)
                node2Field.scoreH = distSq * distSq;
                node2Field.scoreG = tentativeScoreG;
                node2Field.scoreF = node2Field.scoreG + node2Field.scoreH;
                node2Field.cameFrom = {node % m_size, node / m_size};

                // insert and mark as present in open set
                m_openSetWorkingVar.insert(node2);
                node2Field.isInOpenSet = m_boolTrue;
            }
            else if(tentativeScoreG < m_fields[node2].scoreG) {
                // node2 is already in open set, but we have to update it
                // (we have to remove it from the set, update it, and then add it again)

                m_openSetWorkingVar.erase(m_openSetWorkingVar.find(node2));

                auto &node2Field = m_fields[node2];

                node2Field.cameFrom = {node % m_size, node / m_size};
                node2Field.scoreG = tentativeScoreG;
                node2Field.scoreF = node2Field.scoreG + node2Field.scoreH;

                m_openSetWorkingVar.insert(node2);
            }
        }
    }

    return false;
}

void WorldPartFreePosFinder::reconstructPathFindingAlgorithmPath(const engine::IntVec2 &start, engine::IntVec2 current)
{
    TRACK;

    int iterationsGuard{};

    while(start != current) {
        ++iterationsGuard;
        E_DASSERT(iterationsGuard < 1000000, "Probably infinite loop.");

        auto &field = getField(current);
        getField(field.cameFrom).goesTo = current;
        current = field.cameFrom;
    }
}

WorldPartFreePosFinder::Field &WorldPartFreePosFinder::getField(const engine::IntVec2 &tile)
{
    E_DASSERT(tile.x >= 0 && tile.x < m_size && tile.y >= 0 && tile.y < m_size, "Index out of bounds.");

    return m_fields[tile.y * m_size + tile.x];
}

const WorldPartFreePosFinder::Field &WorldPartFreePosFinder::getField(const engine::IntVec2 &tile) const
{
    E_DASSERT(tile.x >= 0 && tile.x < m_size && tile.y >= 0 && tile.y < m_size, "Index out of bounds.");

    return m_fields[tile.y * m_size + tile.x];
}

bool WorldPartFreePosFinder::isInBounds(const engine::IntVec2 &tile) const
{
    return tile.x >= 0 && tile.x < m_size && tile.y >= 0 && tile.y < m_size;
}

bool WorldPartFreePosFinder::isPassThroughAble(const Field &field) const
{
    if(field.height < WorldPart::k_waterHeight)
        return true;

    return !field.isUsed && field.isSlopeWalkable;
}

bool WorldPartFreePosFinder::isPassThroughAble(const engine::IntVec2 &tile) const
{
    if(!isInBounds(tile))
        return false;

    return isPassThroughAble(getField(tile));
}

engine::IntVec2 WorldPartFreePosFinder::realPosToTile(const engine::FloatVec2 &pos) const
{
    return {static_cast <int> (std::floor(pos.x / k_fieldSize)),
            static_cast <int> (std::floor(pos.y / k_fieldSize))};
}

engine::IntVec2 WorldPartFreePosFinder::realPosToTile(const engine::FloatVec3 &pos) const
{
    return realPosToTile({pos.x, pos.z});
}

engine::IntVec2 WorldPartFreePosFinder::getClosestTileInBoundsToTile(const engine::IntVec2 &tile) const
{
    E_DASSERT(m_size, "Size is 0.");

    return {engine::Math::clamp(tile.x, 0, m_size - 1),
            engine::Math::clamp(tile.y, 0, m_size - 1)};
}

const float WorldPartFreePosFinder::k_fieldSize{1.f};
const float WorldPartFreePosFinder::k_maxWalkableSlope{0.4f};
const int WorldPartFreePosFinder::k_maxPathFindingAlgorithmIterations{1600};

} // namespace app
