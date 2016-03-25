#ifndef APP_SPAWN_MANAGER_HPP
#define APP_SPAWN_MANAGER_HPP

#include "../util/Timer.hpp"
#include "engine/util/Trace.hpp"
#include "engine/util/Range.hpp"
#include "PlacementPredicates.hpp"

namespace app
{

class Character;

class SpawnManager : public engine::Tracked <SpawnManager>
{
public:
    void update();

private:
    bool isNight() const;
    void spawn();

    void updateSpawnedAliveMobsContainer();

    static const float k_spawnTimeHoursStartEvening;
    static const float k_spawnTimeHoursEndMorning;
    static const float k_spawnTime;
    static const PlacementPredicates k_spawnPlacementPredicates;
    static const int k_maxSpawnedAliveMobs;

    std::vector <std::shared_ptr <Character>> m_spawnedAliveMobs;

    Timer m_spawnTimer;
};

} // namespace app

#endif // APP_SPAWN_MANAGER_HPP

