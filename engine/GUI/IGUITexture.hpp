#ifndef ENGINE_GUI_I_GUI_TEXTURE_HPP
#define ENGINE_GUI_I_GUI_TEXTURE_HPP

#include "../util/Rect.hpp"
#include "../util/Vec2.hpp"
#include "../util/Color.hpp"

namespace engine { class Color; }

namespace engine
{
namespace GUI
{

class IGUITexture
{
public:
    virtual void draw(const IntRect &rect, const Color &color = Color::k_white) const = 0;
    virtual IntVec2 getSize() const = 0;

    void draw(const IntVec2 &pos, const Color &color = Color::k_white) const;

    virtual ~IGUITexture() = default;
};

} // namespace GUI
} // namespace engine

#endif // ENGINE_GUI_I_GUI_TEXTURE_HPP
