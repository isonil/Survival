#include "IGUITexture.hpp"

namespace engine
{
namespace GUI
{

void IGUITexture::draw(const IntVec2 &pos, const Color &color) const
{
    draw({pos, getSize()}, color);
}

} // namespace GUI
} // namespace engine
