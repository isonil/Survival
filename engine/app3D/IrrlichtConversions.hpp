#ifndef ENGINE_APP_3D_IRRLICHT_CONVERSIONS_HPP
#define ENGINE_APP_3D_IRRLICHT_CONVERSIONS_HPP

#include "../util/Color.hpp"
#include "../util/Vec2.hpp"
#include "../util/Vec3.hpp"
#include "../util/Rect.hpp"

#include <irrlicht/irrlicht.h>

namespace engine
{
namespace app3D
{

class IrrlichtConversions
{
public:
    static irr::video::SColor toColor(const Color &color);
    static irr::video::SColorf toColorf(const Color &color);
    static irr::core::vector2df toVector(const FloatVec2 &vec);
    static irr::core::vector3df toVector(const FloatVec3 &vec);
    static irr::core::vector2di toVector(const IntVec2 &vec);
    static irr::core::vector3di toVector(const IntVec3 &vec);
    static irr::core::rectf toRect(const FloatRect &rect);
    static irr::core::recti toRect(const IntRect &rect);
    static irr::core::dimension2df toDimension(const FloatVec2 &vec);
    static irr::core::dimension2di toDimension(const IntVec2 &vec);
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_IRRLICHT_CONVERSIONS_HPP
