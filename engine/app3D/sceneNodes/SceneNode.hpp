#ifndef ENGINE_APP_3D_SCENE_NODE_HPP
#define ENGINE_APP_3D_SCENE_NODE_HPP

#include "../../util/Vec3.hpp"
#include "../../util/Trace.hpp"
#include "../IIrrlichtObjectsHolder.hpp"

#include <memory>
#include <string>

namespace engine { class AppTime; }

namespace engine
{
namespace app3D
{

class Device;

class SceneNode : public IIrrlichtObjectsHolder, public Tracked <SceneNode>
{
public:
    explicit SceneNode(const std::weak_ptr <Device> &device);
    SceneNode(const SceneNode &) = delete;

    SceneNode &operator = (const SceneNode &) = delete;

    virtual bool wantsEverUpdate() const = 0;
    virtual void update(const FloatVec3 &cameraPos, const AppTime &appTime);

    virtual ~SceneNode() = default;

protected:
    bool deviceExpired() const;
    Device &getDevice_slow();

private:
    std::weak_ptr <Device> m_device;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_SCENE_NODE_HPP
