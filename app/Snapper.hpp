#ifndef APP_SNAPPER_HPP
#define APP_SNAPPER_HPP

#include "engine/util/Vec3.hpp"

#include <array>

namespace app
{

class Structure;
class StructureDef;

class Snapper
{
public:
    static std::vector <std::pair <engine::FloatVec3, engine::FloatVec3>> trySnap(const Structure &first, const StructureDef &secondDef, const engine::FloatVec3 &designatedPos);

private:
    static std::vector <std::pair <engine::FloatVec3, engine::FloatVec3>> floorToFloor(const engine::FloatVec3 &firstSize, const engine::FloatVec3 &secondSize, const engine::FloatVec3 &designatedPos, const Structure &first, const engine::FloatVec3 &secondPosOffset);
    static std::vector <std::pair <engine::FloatVec3, engine::FloatVec3>> wallToWall(const engine::FloatVec3 &firstSize, const engine::FloatVec3 &secondSize, const engine::FloatVec3 &designatedPos, const Structure &first, const engine::FloatVec3 &secondPosOffset);
    static std::vector <std::pair <engine::FloatVec3, engine::FloatVec3>> floorToWall(const engine::FloatVec3 &firstSize, const engine::FloatVec3 &secondSize, const engine::FloatVec3 &designatedPos, const Structure &first, const engine::FloatVec3 &secondPosOffset);
    static std::vector <std::pair <engine::FloatVec3, engine::FloatVec3>> wallToFloor(const engine::FloatVec3 &firstSize, const engine::FloatVec3 &secondSize, const engine::FloatVec3 &designatedPos, const Structure &first, const engine::FloatVec3 &secondPosOffset);

    static engine::FloatVec3 relPosToWorldPos(const engine::FloatVec3 &relPos, const Structure &structure);
    template <std::size_t N> static std::vector <int> getBestHotspots(const engine::FloatVec3 &pos, const Structure &structure, const std::array <engine::FloatVec3, N> &hotspots);
};

template <std::size_t N> std::vector <int> Snapper::getBestHotspots(const engine::FloatVec3 &pos, const Structure &structure, const std::array <engine::FloatVec3, N> &hotspots)
{
    int minIndex{-1};
    float minDist{};

    int minSecondIndex{-1};
    float minSecondDist{};

    for(size_t i = 0; i < hotspots.size(); ++i) {
        const auto &inWorldPos = relPosToWorldPos(hotspots[i], structure);
        float dist{pos.getDistanceSq(inWorldPos)};

        if(dist < minDist || minIndex < 0) {
            minSecondIndex = minIndex;
            minSecondDist = minDist;

            minIndex = i;
            minDist = dist;
        }
        else if(dist < minSecondDist || minSecondIndex < 0) {
            minSecondIndex = i;
            minSecondDist = dist;
        }
    }

    std::vector <int> ret;

    if(minIndex >= 0)
        ret.push_back(minIndex);

    if(minSecondIndex >= 0)
        ret.push_back(minSecondIndex);

    return ret;
}

} // namespace app

#endif // APP_SNAPPER_HPP
