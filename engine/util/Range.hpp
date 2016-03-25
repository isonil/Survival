#ifndef ENGINE_RANGE_HPP
#define ENGINE_RANGE_HPP

#include "DataFile.hpp"
#include "Random.hpp"

namespace engine
{

template <typename T> struct Range;

typedef Range <int> IntRange;
typedef Range <float> FloatRange;

template <typename T> struct Range : public DataFile::Saveable
{
    Range();
    Range(T from, T to); // [from, to]

    bool operator == (const Range &range) const;
    bool operator != (const Range &range) const;
    Range operator + (T val) const;
    Range &operator += (T val);
    Range operator - (T val) const;
    Range &operator -= (T val);
    Range operator * (T val) const;
    Range &operator *= (T val);
    Range operator / (T val) const;
    Range &operator /= (T val);

    void expose(DataFile::Node &node) override;

    Range &set(T from, T to);
    Range &set(const Range &range);
    bool isEmpty() const;
    T randomElement() const;
    T getLength() const;
    bool isInRange(T value) const;

    T from, to;
};

template <typename T> inline Range <T>::Range()
    :   from{}, to{static_cast <T> (-1)}
{
}

template <typename T> inline Range <T>::Range(T from, T to)
    :   from{from}, to{to}
{
}

template <typename T> inline bool Range <T>::operator == (const Range &range) const
{
    return from == range.from && to == range.to;
}

template <typename T> inline bool Range <T>::operator != (const Range &range) const
{
    return !(*this == range);
}

template <typename T> inline Range <T> Range <T>::operator + (T val) const
{
    return {from + val, to + val};
}

template <typename T> inline Range <T> &Range <T>::operator += (T val)
{
    from += val;
    to += val;
    return *this;
}

template <typename T> inline Range <T> Range <T>::operator - (T val) const
{
    return {from - val, to - val};
}

template <typename T> inline Range <T> &Range <T>::operator -= (T val)
{
    from -= val;
    to -= val;
    return *this;
}

template <typename T> inline Range <T> Range <T>::operator * (T val) const
{
    return {from * val, to * val};
}

template <typename T> inline Range <T> &Range <T>::operator *= (T val)
{
    from *= val;
    to *= val;
    return *this;
}

template <typename T> inline Range <T> Range <T>::operator / (T val) const
{
    return {from / val, to / val};
}

template <typename T> inline Range <T> &Range <T>::operator /= (T val)
{
    from /= val;
    to /= val;
    return *this;
}

template <typename T> inline void Range <T>::expose(DataFile::Node &node)
{
    node.var(from, "from");
    node.var(to, "to");
}

template <typename T> inline Range <T> &Range <T>::set(T from, T to)
{
    this->from = from;
    this->to = to;
    return *this;
}

template <typename T> inline Range <T> &Range <T>::set(const Range &range)
{
    from = range.from;
    to = range.to;
    return *this;
}

template <typename T> inline bool Range <T>::isEmpty() const
{
    return from > to;
}

template <typename T> inline T Range <T>::randomElement() const
{
    if(isEmpty())
        return from;

    return Random::rangeInclusive(from, to);
}

template <typename T> inline T Range <T>::getLength() const
{
    if(isEmpty())
        return 0;

    return to - from;
}

template <typename T> inline bool Range <T>::isInRange(T value) const
{
    return value >= from && value <= to;
}

} // namespace engine

#endif // ENGINE_RANGE_HPP
