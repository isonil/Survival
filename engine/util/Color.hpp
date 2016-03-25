#ifndef ENGINE_COLOR_HPP
#define ENGINE_COLOR_HPP

#include "DataFile.hpp"

namespace engine
{

struct Color : public DataFile::Saveable
{
    Color();
    Color(float r, float g, float b);
    Color(float r, float g, float b, float a);
    Color(int r, int g, int b);
    Color(int r, int g, int b, int a);

    Color operator + (float val) const;
    Color &operator += (float val);
    Color operator - (float val) const;
    Color &operator -= (float val);
    Color operator + (const Color &color) const;
    Color &operator += (const Color &color);
    Color operator - (const Color &color) const;
    Color &operator -= (const Color &color);
    Color operator * (float val) const;
    Color &operator *= (float val);
    Color operator / (float val) const;
    Color &operator /= (float val);
    bool operator == (const Color &col) const;
    bool operator != (const Color &col) const;

    void expose(DataFile::Node &node) override;

    Color &normalize();
    Color mixed(const Color &with, float f) const;
    Color brightened(float val) const;
    Color brightened(int val) const;
    Color darkened(float val) const;
    Color darkened(int val) const;
    Color changedAlpha(float val) const;
    Color changedRed(float val) const;
    Color changedGreen(float val) const;
    Color changedBlue(float val) const;

    static const Color k_black;
    static const Color k_white;
    static const Color k_gray;
    static const Color k_red;
    static const Color k_green;
    static const Color k_blue;
    static const Color k_yellow;
    static const Color k_cyan;
    static const Color k_darkRed;
    static const Color k_darkGreen;
    static const Color k_darkBlue;
    static const Color k_transparentBlack;

    float r, g, b, a;
};

} // namespace engine

#endif // ENGINE_COLOR_HPP
