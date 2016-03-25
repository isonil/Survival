#include "SpawnManager.hpp"

#include "../itemContainers/SingleSlotItemContainer.hpp"
#include "../entities/Character.hpp"
#include "../entities/Item.hpp"
#include "../thisPlayer/ThisPlayer.hpp"
#include "../defs/WorldPartDef.hpp"
#include "../defs/DefsCache.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "World.hpp"
#include "WorldPart.hpp"

namespace app
{

void SpawnManager::update()
{
    TRACK;

    updateSpawnedAliveMobsContainer();

    if(!isNight())
        return;

    if(m_spawnTimer.passed()) {
        m_spawnTimer.set(k_spawnTime);
        spawn();
    }
}

bool SpawnManager::isNight() const
{
    float currentHours{Global::getCore().getWorld().getDateTimeManager().getTime().getHoursAsFloat()};

    return currentHours >= k_spawnTimeHoursStartEvening || currentHours <= k_spawnTimeHoursEndMorning;
}

void SpawnManager::spawn()
{
    TRACK;

    if(m_spawnedAliveMobs.size() >= static_cast <size_t> (k_maxSpawnedAliveMobs))
        return;

    auto &core = Global::getCore();
    auto &thisPlayer = core.getThisPlayer();
    auto &world = core.getWorld();
    const auto &defsCache = core.getDefsCache();

    auto *worldPart = world.getWorldPart(thisPlayer.getCharacter().getInWorldPosition());

    if(worldPart) {
        const auto &mobs = worldPart->getDef().getMobs();

        if(!mobs.empty()) {
            const auto &mob = engine::Random::randomElementByWeight(mobs, [](const auto &elem) {
                return elem.getSpawnRateWeight();
            });

            const auto &pos = worldPart->getRandomPosMatching_worldPos(k_spawnPlacementPredicates);

            if(pos) {
                const auto &item = std::make_shared <Item> (world.getUniqueEntityID(), mob.getInHandsItemDefPtr());
                const auto &character = std::make_shared <Character> (world.getUniqueEntityID(), mob.getCharacterDefPtr(), false);

                character->setFactionDef(mob.getFactionDefPtr());
                character->setInWorldPosition(*pos);
                character->getInventory().getHandsItemContainer().tryAddItem(item);

                world.addEntity(character);

                m_spawnedAliveMobs.push_back(character);
            }
        }
    }
}

void SpawnManager::updateSpawnedAliveMobsContainer()
{
    TRACK;

    for(size_t i = 0; i < m_spawnedAliveMobs.size();) {
        E_DASSERT(m_spawnedAliveMobs[i], "Character is nullptr.");

        if(m_spawnedAliveMobs[i]->isKilled() || !m_spawnedAliveMobs[i]->isInWorld()) {
            std::swap(m_spawnedAliveMobs[i], m_spawnedAliveMobs.back());
            m_spawnedAliveMobs.pop_back();
        }
        else
            ++i;
    }
}

const float SpawnManager::k_spawnTimeHoursStartEvening{20.f};
const float SpawnManager::k_spawnTimeHoursEndMorning{4.f};
const float SpawnManager::k_spawnTime{25000.f};
const PlacementPredicates SpawnManager::k_spawnPlacementPredicates{{0.f, 0.2f}, true, false};
const int SpawnManager::k_maxSpawnedAliveMobs{5};

} // namespace app
