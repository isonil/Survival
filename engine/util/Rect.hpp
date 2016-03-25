#ifndef ENGINE_RECT_HPP
#define ENGINE_RECT_HPP

#include "DataFile.hpp"
#include "Vec2.hpp"

namespace engine
{

template <typename T> struct Rect;

typedef Rect <int> IntRect;
typedef Rect <float> FloatRect;

template <typename T> struct Rect : public DataFile::Saveable
{
    Rect() = default;
    Rect(const Vec2 <T> &pos, const Vec2 <T> &size);
    Rect(T x, T y, T w, T h);

    bool operator == (const Rect &rect) const;
    bool operator != (const Rect &rect) const;

    void expose(DataFile::Node &node) override;

    bool contains(const Vec2 <T> &vec) const;
    bool fullyContains(const Rect &rect) const;
    bool intersects(const Rect &rect) const;
    bool isEmpty() const;
    T getMaxX() const;
    T getMaxY() const;
    Rect getInnerRect(T padding) const;
    Rect getOuterRect(T padding) const;
    Rect getInnerSquare() const;
    Rect &move(T x, T y);
    Rect &move(const Vec2 <T> &offset);
    Rect moved(T x, T y) const;
    Rect moved(const Vec2 <T> &offset) const;
    Rect movedX(T x) const;
    Rect movedY(T y) const;
    Rect movedTopBorder(T by) const;
    Rect movedBottomBorder(T by) const;
    Rect movedLeftBorder(T by) const;
    Rect movedRightBorder(T by) const;
    Rect fittedToSquareWithAspect(const Vec2 <T> &squarePos, T squareSize) const;

    Vec2 <T> pos;
    Vec2 <T> size;
};

template <typename T> inline Rect <T>::Rect(const Vec2 <T> &pos, const Vec2 <T> &size)
    : pos{pos}, size{size}
{
}

template <typename T> inline Rect <T>::Rect(T x, T y, T w, T h)
    : pos{x, y}, size{w, h}
{
}

template <typename T> inline bool Rect <T>::operator == (const Rect &rect) const
{
    return pos == rect.pos && size == rect.size;
}

template <typename T> inline bool Rect <T>::operator != (const Rect &rect) const
{
    return !(*this == rect);
}

template <typename T> inline void Rect <T>::expose(DataFile::Node &node)
{
    node.var(pos, "pos");
    node.var(size, "size");
}

template <typename T> inline bool Rect <T>::contains(const Vec2 <T> &vec) const
{
    return vec.x >= pos.x && vec.y >= pos.y &&
           vec.x < pos.x + size.x && vec.y < pos.y + size.y;
}

template <typename T> inline bool Rect <T>::fullyContains(const Rect &rect) const
{
    return rect.pos.x >= pos.x && rect.pos.y >= pos.y &&
           rect.pos.x + rect.size.x <= pos.x + size.x &&
           rect.pos.y + rect.size.y <= pos.y + size.y;
}

template <typename T> inline bool Rect <T>::intersects(const Rect &rect) const
{
    return pos.x < rect.pos.x + rect.size.x && pos.x + size.x > rect.pos.x &&
           pos.y < rect.pos.y + rect.size.y && pos.y + size.y > rect.pos.y;
}

template <typename T> inline bool Rect <T>::isEmpty() const
{
    return size.x <= 0 || size.y <= 0;
}

template <typename T> inline T Rect <T>::getMaxX() const
{
    return pos.x + size.x;
}

template <typename T> inline T Rect <T>::getMaxY() const
{
    return pos.y + size.y;
}

template <typename T> inline Rect <T> Rect <T>::getInnerRect(T padding) const
{
    auto ret = *this;

    T paddingX{padding};
    T paddingY{padding};

    if(ret.size.x < paddingX * 2)
        paddingX = ret.size.x / 2;

    if(ret.size.y < paddingY * 2)
        paddingY = ret.size.y / 2;

    ret.pos.x += paddingX;
    ret.size.x -= paddingX * 2;

    ret.pos.y += paddingY;
    ret.size.y -= paddingY * 2;

    return ret;
}

template <typename T> inline Rect <T> Rect <T>::getOuterRect(T padding) const
{
    auto ret = *this;

    ret.pos.x -= padding;
    ret.size.x += padding * 2;

    ret.pos.y -= padding;
    ret.size.y += padding * 2;

    return ret;
}

template <typename T> inline Rect <T> Rect <T>::getInnerSquare() const
{
    auto ret = *this;

    if(size.x > size.y) {
        ret.pos.x += (size.x - size.y) / 2;
        ret.size.x = size.y;
    }
    else if(size.y > size.x) {
        ret.pos.y += (size.y - size.x) / 2;
        ret.size.y = size.x;
    }

    return ret;
}

template <typename T> inline Rect <T> &Rect <T>::move(T x, T y)
{
    this->pos.x += x;
    this->pos.y += y;
    return *this;
}

template <typename T> inline Rect <T> &Rect <T>::move(const Vec2 <T> &offset)
{
    this->pos += offset;
    return *this;
}

template <typename T> inline Rect <T> Rect <T>::moved(T x, T y) const
{
    return {pos.x + x, pos.y + y, size.x, size.y};
}

template <typename T> inline Rect <T> Rect <T>::moved(const Vec2 <T> &offset) const
{
    return {pos + offset, size};
}

template <typename T> inline Rect <T> Rect <T>::movedX(T x) const
{
    return {pos.x + x, pos.y, size.x, size.y};
}

template <typename T> inline Rect <T> Rect <T>::movedY(T y) const
{
    return {pos.x, pos.y + y, size.x, size.y};
}

template <typename T> inline Rect <T> Rect <T>::movedTopBorder(T by) const
{
    auto ret = *this;

    if(by > ret.size.y)
        by = ret.size.y;

    ret.pos.y += by;
    ret.size.y -= by;

    return ret;
}

template <typename T> inline Rect <T> Rect <T>::movedBottomBorder(T by) const
{
    auto ret = *this;

    ret.size.y += by;

    if(ret.size.y < 0)
        ret.size.y = 0;

    return ret;
}

template <typename T> inline Rect <T> Rect <T>::movedLeftBorder(T by) const
{
    auto ret = *this;

    if(by > ret.size.x)
        by = ret.size.x;

    ret.pos.x += by;
    ret.size.x -= by;

    return ret;
}

template <typename T> inline Rect <T> Rect <T>::movedRightBorder(T by) const
{
    auto ret = *this;

    ret.size.x += by;

    if(ret.size.x < 0)
        ret.size.x = 0;

    return ret;
}

template <typename T> inline Rect <T> Rect <T>::fittedToSquareWithAspect(const Vec2 <T> &squarePos, T squareSize) const
{
    if(squareSize <= 0)
        return {squarePos, {}};

    auto ret = *this;

    auto aspect = static_cast <float> (size.x) / size.y;

    if(ret.size.x > ret.size.y) {
        ret.size.x = squareSize;
        ret.size.y = ret.size.x / aspect;
    }
    else {
        ret.size.y = squareSize;
        ret.size.x = ret.size.y * aspect;
    }

    ret.pos.x = squarePos.x + (squareSize - ret.size.x) / 2;
    ret.pos.y = squarePos.y + (squareSize - ret.size.y) / 2;

    return ret;
}

} // namespace engine

#endif // ENGINE_RECT_HPP
