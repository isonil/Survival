#ifndef ENGINE_APP_3D_CONE_TWIST_CONSTRAINT_HPP
#define ENGINE_APP_3D_CONE_TWIST_CONSTRAINT_HPP

#include "../../util/Vec3.hpp"
#include "../../util/Trace.hpp"

class btConeTwistConstraint;
class btDynamicsWorld;

namespace engine
{
namespace app3D
{

class RigidBody;

class ConeTwistConstraint : public Tracked <ConeTwistConstraint>
{
public:
    ConeTwistConstraint(const std::weak_ptr <btDynamicsWorld> &dynamicsWorld, const std::shared_ptr <RigidBody> &rigidBody1, const std::shared_ptr <RigidBody> &rigidBody2, const FloatVec3 &connectionPoint1, const FloatVec3 &connectionPoint2);

    ~ConeTwistConstraint();

private:
    std::weak_ptr <btDynamicsWorld> m_dynamicsWorld;
    std::unique_ptr <btConeTwistConstraint> m_coneTwistConstraint;

    std::shared_ptr <RigidBody> m_rigidBody1;
    std::shared_ptr <RigidBody> m_rigidBody2;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_CONE_TWIST_CONSTRAINT_HPP
