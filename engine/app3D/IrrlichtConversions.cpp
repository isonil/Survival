#include "IrrlichtConversions.hpp"

namespace engine
{
namespace app3D
{

irr::video::SColor IrrlichtConversions::toColor(const Color &color)
{
    return {static_cast <unsigned int> (color.a * 255.f),
            static_cast <unsigned int> (color.r * 255.f),
            static_cast <unsigned int> (color.g * 255.f),
            static_cast <unsigned int> (color.b * 255.f)};
}

irr::video::SColorf IrrlichtConversions::toColorf(const Color &color)
{
    return {color.r, color.g, color.b, color.a};
}

irr::core::vector2df IrrlichtConversions::toVector(const FloatVec2 &vec)
{
    return {vec.x, vec.y};
}

irr::core::vector3df IrrlichtConversions::toVector(const FloatVec3 &vec)
{
    return {vec.x, vec.y, vec.z};
}

irr::core::vector2di IrrlichtConversions::toVector(const IntVec2 &vec)
{
    return {vec.x, vec.y};
}

irr::core::vector3di IrrlichtConversions::toVector(const IntVec3 &vec)
{
    return {vec.x, vec.y, vec.z};
}

irr::core::rectf IrrlichtConversions::toRect(const FloatRect &rect)
{
    return {rect.pos.x, rect.pos.y, rect.pos.x + rect.size.x, rect.pos.y + rect.size.y};
}

irr::core::recti IrrlichtConversions::toRect(const IntRect &rect)
{
    return {rect.pos.x, rect.pos.y, rect.pos.x + rect.size.x, rect.pos.y + rect.size.y};
}

irr::core::dimension2df IrrlichtConversions::toDimension(const FloatVec2 &vec)
{
    return {vec.x, vec.y};
}

irr::core::dimension2di toDimension(const IntVec2 &vec)
{
    return {vec.x, vec.y};
}

} // namespace app3D
} // namespace engine
