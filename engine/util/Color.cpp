#include "Color.hpp"

#include "Math.hpp"

namespace engine
{

Color::Color()
    : r{}, g{}, b{}, a{1.f}
{
}

Color::Color(float r, float g, float b)
    : r{r}, g{g}, b{b}, a{1.f}
{
    normalize();
}

Color::Color(float r, float g, float b, float a)
    : r{r}, g{g}, b{b}, a{a}
{
    normalize();
}

Color::Color(int r, int g, int b)
    : r{r / 255.f}, g{g / 255.f}, b{b / 255.f}, a{1.f}
{
    normalize();
}

Color::Color(int r, int g, int b, int a)
    : r{r / 255.f}, g{g / 255.f}, b{b / 255.f}, a{a / 255.f}
{
    normalize();
}

Color Color::operator + (float val) const
{
    return {r + val, g + val, b + val, a};
}

Color &Color::operator += (float val)
{
    r += val;
    g += val;
    b += val;
    normalize();

    return *this;
}

Color Color::operator - (float val) const
{
    return {r - val, g - val, b - val, a};
}

Color &Color::operator -= (float val)
{
    r -= val;
    g -= val;
    b -= val;
    normalize();

    return *this;
}

Color Color::operator + (const Color &color) const
{
    return {r + color.r * color.a, g + color.g * color.a, b + color.b * color.a, a};
}

Color &Color::operator += (const Color &color)
{
    r += color.r * color.a;
    g += color.g * color.a;
    b += color.b * color.a;
    normalize();

    return *this;
}

Color Color::operator - (const Color &color) const
{
    return {r - color.r * color.a, g - color.g * color.a, b - color.b * color.a};
}

Color &Color::operator -= (const Color &color)
{
    r -= color.r * color.a;
    g -= color.g * color.a;
    b -= color.b * color.a;
    normalize();

    return *this;
}

Color Color::operator * (float val) const
{
    return {r * val, g * val, b * val};
}

Color &Color::operator *= (float val)
{
    r *= val;
    g *= val;
    b *= val;
    normalize();

    return *this;
}

Color Color::operator / (float val) const
{
    return {r / val, g / val, b / val};
}

Color &Color::operator /= (float val)
{
    r /= val;
    g /= val;
    b /= val;
    normalize();

    return *this;
}

bool Color::operator == (const Color &col) const
{
    return r == col.r && g == col.g && b == col.b && a == col.a;
}

bool Color::operator != (const Color &col) const
{
    return !(*this == col);
}

void Color::expose(DataFile::Node &node)
{
    node.var(r, "r");
    node.var(g, "g");
    node.var(b, "b");
    node.var(a, "a", 1.f);
}

Color &Color::normalize()
{
    r = Math::clamp01(r);
    g = Math::clamp01(g);
    b = Math::clamp01(b);
    a = Math::clamp01(a);

    return *this;
}

Color Color::mixed(const Color &with, float f) const
{
    return {r * (1.f - f) + with.r * f,
            g * (1.f - f) + with.g * f,
            b * (1.f - f) + with.b * f,
            a * (1.f - f) + with.a * f};
}

Color Color::brightened(float val) const
{
    return {r + val, g + val, b + val, a};
}

Color Color::brightened(int val) const
{
    return {r + val / 255.f, g + val / 255.f, b + val / 255.f, a};
}

Color Color::darkened(float val) const
{
    return {r - val, g - val, b - val, a};
}

Color Color::darkened(int val) const
{
    return {r - val / 255.f, g - val / 255.f, b - val / 255.f, a};
}

Color Color::changedAlpha(float val) const
{
    return {r, g, b, val};
}

Color Color::changedRed(float val) const
{
    return {val, g, b, a};
}

Color Color::changedGreen(float val) const
{
    return {r, val, b, a};
}

Color Color::changedBlue(float val) const
{
    return {r, g, val, a};
}

const Color Color::k_black{0.f, 0.f, 0.f};
const Color Color::k_white{1.f, 1.f, 1.f};
const Color Color::k_gray{0.5f, 0.5f, 0.5f};
const Color Color::k_red{1.f, 0.f, 0.f};
const Color Color::k_green{0.f, 1.f, 0.f};
const Color Color::k_blue{0.f, 0.f, 1.f};
const Color Color::k_yellow{1.f, 1.f, 0.f};
const Color Color::k_cyan{0.f, 1.f, 1.f};
const Color Color::k_darkRed{0.5f, 0.f, 0.f};
const Color Color::k_darkGreen{0.f, 0.5f, 0.f};
const Color Color::k_darkBlue{0.f, 0.f, 0.5f};
const Color Color::k_transparentBlack{0.f, 0.f, 0.f, 0.f};

} // namespace engine
