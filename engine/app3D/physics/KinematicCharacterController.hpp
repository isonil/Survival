#ifndef ENGINE_APP_3D_KINEMATIC_CHARACTER_CONTROLLER_HPP
#define ENGINE_APP_3D_KINEMATIC_CHARACTER_CONTROLLER_HPP

#include "../../util/Trace.hpp"
#include "../../util/Vec3.hpp"
#include "CollisionShape.hpp"

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>

#include <memory>

namespace engine
{
namespace app3D
{

// this class is deprecated, use DynamicCharacterController instead

class KinematicCharacterController : public Tracked <KinematicCharacterController>
{
public:
    KinematicCharacterController(const std::weak_ptr <btDynamicsWorld> &dynamicsWorld, const std::shared_ptr <CollisionShape> &shape, float mass, const FloatVec3 &posOffset);

    void setPosition(const FloatVec3 &pos);
    void setMovement(const FloatVec3 &movement);
    FloatVec3 getPosition() const;

    ~KinematicCharacterController();

private:
    static const float k_stepHeight;

    std::weak_ptr <btDynamicsWorld> m_dynamicsWorld;
    std::unique_ptr <btKinematicCharacterController> m_character;
    std::unique_ptr <btPairCachingGhostObject> m_ghostObject;
    std::shared_ptr <CollisionShape> m_shape;
    float m_mass;
    FloatVec3 m_posOffset;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_KINEMATIC_CHARACTER_CONTROLLER_HPP
