#ifndef ENGINE_APP_3D_ARMATURE_HPP
#define ENGINE_APP_3D_ARMATURE_HPP

#include <unordered_map>
#include <memory>
#include <vector>

namespace irr { namespace scene {class IBoneSceneNode; } }

namespace engine
{
namespace app3D
{

class PhysicsManager;
class Model;
class RigidBody;
class ConeTwistConstraint;

class Armature
{
public:
    Armature(PhysicsManager &physicsManager, const Model &model);

    void updateModelWithArmature(Model &model);

private:
    void constructArmature(PhysicsManager &physicsManager, irr::scene::IBoneSceneNode &joint);

    std::unordered_map <int, std::pair <std::shared_ptr <RigidBody>, float>> m_rigidBodies;
    std::vector <std::shared_ptr <ConeTwistConstraint>> m_coneTwistConstraints;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_ARMATURE_HPP
