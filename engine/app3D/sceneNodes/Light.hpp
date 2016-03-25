#ifndef ENGINE_APP_3D_LIGHT_HPP
#define ENGINE_APP_3D_LIGHT_HPP

#include "../../util/Trace.hpp"
#include "../../util/Color.hpp"
#include "../../util/Vec3.hpp"
#include "SceneNode.hpp"

#include <irrlicht/irrlicht.h>

namespace engine
{
namespace app3D
{

class LightDef;

class Light : public SceneNode, public Tracked <Light>
{
public:
    Light(const std::shared_ptr <LightDef> &lightDef, const std::weak_ptr <Device> &device);

    void dropIrrObjects() override;
    void reloadIrrObjects() override;
    bool wantsEverUpdate() const override;
    void update(const FloatVec3 &cameraPos, const AppTime &appTime) override;

    LightDef &getDef() const;

    void setPosition(const FloatVec3 &pos);
    void setDirectionalLightDirection(const FloatVec3 &dir);
    void setColor(const Color &color);
    const FloatVec3 &getPosition() const;
    const Color &getColor() const;
    float getCurrentRadius() const;

    ~Light() override;

    static const float k_quadricAttenuationConstant;

private:
    void createRender();
    void removeCurrentRender();
    void updateRadiusDistortion(const AppTime &appTime);

    struct
    {
        irr::scene::ILightSceneNode *lightNode{};
    } m_currentRender;

    std::shared_ptr <LightDef> m_lightDef;

    FloatVec3 m_pos;
    FloatVec3 m_directionalLightDirection;
    Color m_color;
    float m_radiusDistortion;
    float m_currentRadiusDistortionTarget;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_LIGHT_HPP
