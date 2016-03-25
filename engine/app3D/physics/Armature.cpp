#include "Armature.hpp"

#include "../managers/PhysicsManager.hpp"
#include "../sceneNodes/Model.hpp"
#include "RigidBody.hpp"
#include "SphereShape.hpp"
#include "CapsuleShape.hpp"
#include "ConeTwistConstraint.hpp"

#include <irrlicht.h>

namespace engine
{
namespace app3D
{

Armature::Armature(PhysicsManager &physicsManager, const Model &model)
{
    auto *joint = model.getJoint("Armature");

    if(joint) {
        auto rigidBody = physicsManager.addRigidBody(std::make_shared <SphereShape> (0.5f), 10.f);

        const auto &jointPos = joint->getAbsolutePosition();
        rigidBody->setPosition({jointPos.X, jointPos.Y, jointPos.Z});

        m_rigidBodies.emplace(joint->getBoneIndex(), std::make_pair(std::move(rigidBody), 0.f));

        constructArmature(physicsManager, *joint);
    }
}

void Armature::updateModelWithArmature(Model &model)
{
    for(const auto &elem : m_rigidBodies) {
        const auto &rigidBody = elem.second.first;

        model.setJointPosition(elem.first, rigidBody->getPosition());
        model.setJointRotation(elem.first, rigidBody->getRotation());

        // TODO!
        std::cout << model.getJoint(elem.first)->getName() << " " << rigidBody->getPosition().x << " " << rigidBody->getPosition().y << " " << rigidBody->getPosition().z << std::endl;
    }
}

void Armature::constructArmature(PhysicsManager &physicsManager, irr::scene::IBoneSceneNode &joint)
{
    for(auto *child : joint.getChildren()) {
        auto *boneChild = dynamic_cast <irr::scene::IBoneSceneNode*> (child);

        if(!boneChild)
            continue;

        const auto &jointAbsPos = joint.getAbsolutePosition();
        const auto &childAbsPos = child->getAbsolutePosition();
        const auto &horizontalAngle = (childAbsPos - jointAbsPos).getHorizontalAngle();
        const auto &midPoint = (jointAbsPos + childAbsPos) / 2.f;

        float distToParent{child->getPosition().getLength()};
        auto rigidBody = physicsManager.addRigidBody(std::make_shared <CapsuleShape> (0.2f, distToParent), 10.f);
        rigidBody->setPosition({midPoint.X, midPoint.Y, midPoint.Z});
        rigidBody->setRotation({horizontalAngle.X, horizontalAngle.Y, horizontalAngle.Z});
        m_rigidBodies.emplace(boneChild->getBoneIndex(), std::make_pair(rigidBody, distToParent));

        const auto &parent = m_rigidBodies.find(joint.getBoneIndex());
        E_DASSERT(parent != m_rigidBodies.end(), "Parent not added.");
        auto constraint = physicsManager.addConeTwistConstraint(parent->second.first, rigidBody, {0.f, parent->second.second / 2.f, 0.f}, {0.f, -distToParent / 2.f, 0.f});
        m_coneTwistConstraints.emplace_back(std::move(constraint));
        constructArmature(physicsManager, *boneChild);
    }
}

} // namespace app3D
} // namespace engine
