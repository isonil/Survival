#ifndef ENGINE_APP_3D_COLLISION_FILTER_HPP
#define ENGINE_APP_3D_COLLISION_FILTER_HPP

namespace engine
{
namespace app3D
{

enum class CollisionFilter : short int
{
    // note: these values should be compatible with btBroadphaseProxy::CollisionFilterGroups

    // note: A must collide with B, and B must collide with A, otherwise there is no collision
    // RigidBodies are set to collide with everything
    // so it's up to GhostObjects and CollisionDetectors to decide what to collide with

    // none:
    None = 0,

    // real:
    Default = 1, // dynamic bodies
    Static = 2, // static bodies
    Kinematic = 4, // kinematic bodies
    Debris = 8,
    Character = 32, // characters bodies - usually with capsule shape (note that character bodies are usually also dynamic bodies)

    // ghost:
    GhostMisc = 512, // some kind of ghost object, without any imposed purpose (can be used to detect bodies)
    SensorTrigger = 16, // used for collision detectors detecting characters (trigger zones)
    Water = 64,
    CharacterHitBox = 128,

    // special:
    Raycast = 256,
    All = -1,
    AllReal = Default | Static | Kinematic | Debris | Character | Raycast
};

CollisionFilter operator | (const CollisionFilter &lhs, const CollisionFilter &rhs);
CollisionFilter operator & (const CollisionFilter &lhs, const CollisionFilter &rhs);
CollisionFilter operator ^ (const CollisionFilter &lhs, const CollisionFilter &rhs);

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_COLLISION_FILTER_HPP
