#include "LightDef.hpp"

namespace engine
{
namespace app3D
{

LightDef::LightDef()
    : m_isDirectional{},
      m_radius{1.f},
      m_maxRadiusDistortion{}
{
}

void LightDef::expose(DataFile::Node &node)
{
    base::expose(node);

    node.var(m_isDirectional, "isDirectional", {});
    node.var(m_initialColor, "initialColor");
    node.var(m_radius, "radius", 1.f);
    node.var(m_maxRadiusDistortion, "maxRadiusDistortion", {});

    if(node.getActivityType() == DataFile::Activity::Type::Loading) {
        if(m_radius <= 0.f)
            throw Exception{"Radius must be a positive value."};

        if(m_maxRadiusDistortion < 0.f)
            throw Exception{"Max radius distortion must be >= 0."};
    }
}

void LightDef::dropIrrObjects()
{
}

void LightDef::reloadIrrObjects()
{
}

bool LightDef::isDirectional() const
{
    return m_isDirectional;
}

const Color &LightDef::getInitialColor() const
{
    return m_initialColor;
}

float LightDef::getRadius() const
{
    return m_radius;
}

float LightDef::getMaxRadiusDistortion() const
{
    return m_maxRadiusDistortion;
}

} // namespace app3D
} // namespace engine
