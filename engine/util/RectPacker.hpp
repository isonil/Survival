#ifndef ENGINE_RECT_PACKER_HPP
#define ENGINE_RECT_PACKER_HPP

#include "Trace.hpp"
#include "Rect.hpp"

#include <set>
#include <vector>

namespace engine
{

class RectPacker : public Tracked <RectPacker>
{
public:
    struct RectInfo
    {
        RectInfo();
        RectInfo(int index, const IntRect &rect);

        bool operator () (const RectInfo &left, const RectInfo &right) const;
        bool operator < (const RectInfo &right) const;

        int index;
        IntRect rect;
    };

    explicit RectPacker(int size);

    void add(int w, int h);
    const IntRect &get(int index) const;
    std::vector <RectInfo> getAll() const;
    bool pack();

private:
    static bool sortByIndex(const RectInfo &left, const RectInfo &right);

    int m_size;
    std::set <RectInfo, RectInfo> m_freeZones;
    std::vector <RectInfo> m_rects;
};

} // namespace engine

#endif // ENGINE_RECT_PACKER_HPP
