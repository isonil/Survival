#ifndef ENGINE_APP_3D_LIGHT_DEF_HPP
#define ENGINE_APP_3D_LIGHT_DEF_HPP

#include "../../util/Def.hpp"
#include "../../util/Color.hpp"
#include "ResourceDef.hpp"

namespace engine
{
namespace app3D
{

class LightDef : public ResourceDef, public Tracked <LightDef>
{
public:
    LightDef();

    void expose(DataFile::Node &node) override;

    void dropIrrObjects() override;
    void reloadIrrObjects() override;

    bool isDirectional() const;
    const Color &getInitialColor() const;
    float getRadius() const;
    float getMaxRadiusDistortion() const;

private:
    using base = ResourceDef;

    bool m_isDirectional;
    Color m_initialColor;
    float m_radius;
    float m_maxRadiusDistortion;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_LIGHT_DEF_HPP
