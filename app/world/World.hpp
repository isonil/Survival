#ifndef APP_WORLD_HPP
#define APP_WORLD_HPP

#include "engine/util/Rect.hpp"
#include "engine/util/Trace.hpp"
#include "engine/util/Vec2.hpp"
#include "engine/util/Vec3.hpp"
#include "engine/util/Music.hpp"
#include "GroundType.hpp"
#include "DateTimeManager.hpp"
#include "SpawnManager.hpp"

#include <vector>
#include <memory>
#include <unordered_map>

namespace engine { namespace app3D { class Water; class RigidBody; class Settings; } }

namespace app
{

class Structure;
class WorldPart;
class Entity;
class ElectricitySystem;

class World : public engine::Tracked <World>
{
public:
    World(const engine::app3D::Settings &settings);

    void update();

    void addEntity(const std::shared_ptr <Entity> &entity);
    void removeEntity(int entityID);
    bool entityExists(int entityID) const;
    Entity &getEntity(int entityID) const;
    template <typename T> T &getEntityAndCast(int entityID) const;
    const std::shared_ptr <Entity> &getEntityPtr(int entityID) const;
    template <typename T> const std::shared_ptr <T> &getEntityPtrAndCast(int entityID) const;

    int getUniqueEntityID();
    WorldPart *getWorldPart(const engine::FloatVec2 &pos) const;
    WorldPart *getWorldPart(const engine::FloatVec3 &pos) const;
    float getHeight(const engine::FloatVec2 &pos) const;
    float getHeight(const engine::FloatVec3 &pos) const;
    float getSlope(const engine::FloatVec2 &pos) const;
    float getSlope(const engine::FloatVec3 &pos) const;
    GroundType getGroundType(const engine::FloatVec2 &pos) const;
    GroundType getGroundType(const engine::FloatVec3 &pos) const;
    DateTimeManager &getDateTimeManager();
    const DateTimeManager &getDateTimeManager() const;
    const engine::FloatRect &getBounds() const;
    const engine::FloatVec2 &getPlayerStartingPosition() const;

    void forEachEntity(std::function <void(Entity &)> func) const;
    void playAmbientMusic(bool play);

    std::shared_ptr <ElectricitySystem> addElectricitySystem(std::shared_ptr <Structure> structureMember);
    const std::vector <std::shared_ptr <ElectricitySystem>> &getElectricitySystems() const;

    const std::unordered_map <int, std::shared_ptr <Structure>> &getStructuresUsingElectricity() const;

    ~World();

private:
    void useWorldPartFreePosFinderFieldAt(const engine::FloatVec2 &pos);
    void useWorldPartFreePosFinderFieldAt(const engine::FloatVec3 &pos);
    void updateElectricitySystems();

    void addToQuickAccessCachedEntities(const std::shared_ptr <Entity> &entity);
    void removeFromQuickAccessCachedEntities(const Entity &entity);

    static const std::string k_birdsAmbiencePath;

    DateTimeManager m_dateTimeManager;
    SpawnManager m_spawnManager;
    std::shared_ptr <engine::app3D::RigidBody> m_staticInfinitePlane;
    std::unordered_map <int, std::shared_ptr <Entity>> m_entities_wantUpdate;
    std::unordered_map <int, std::shared_ptr <Entity>> m_entities_dontWantUpdate;
    std::vector <std::unique_ptr <WorldPart>> m_worldParts;
    std::vector <std::shared_ptr <ElectricitySystem>> m_electricitySystems;
    int m_uniqueEntityID;
    engine::Music m_birdsAmbience;
    engine::FloatRect m_bounds;
    engine::FloatVec2 m_playerStartingPosition;

    // quick-access cached entities
    std::unordered_map <int, std::shared_ptr <Structure>> m_structuresUsingElectricity;
};

template <typename T> T &World::getEntityAndCast(int entityID) const
{
    auto &entity = getEntity(entityID);
    auto *entityCast = dynamic_cast <T*> (&entity);

    if(!entityCast)
        throw engine::Exception{"Could not cast entity to desired type."};

    return *entityCast;
}

template <typename T> const std::shared_ptr <T> &World::getEntityPtrAndCast(int entityID) const
{
    const auto &entityPtr = getEntityPtr(entityID);
    const auto &entityCast = std::dynamic_pointer_cast <T> (entityPtr);

    if(!entityCast)
        throw engine::Exception{"Could not cast entity to desired type."};

    return entityCast;
}

} // namespace app

#endif // APP_WORLD_HPP
