#include "RectPacker.hpp"

#include "Exception.hpp"

#include <algorithm>

namespace engine
{

RectPacker::RectInfo::RectInfo()
    : index{}
{
}

RectPacker::RectInfo::RectInfo(int index, const IntRect &rect)
    : index{index},
      rect{rect}
{
}

bool RectPacker::RectInfo::operator () (const RectInfo &left, const RectInfo &right) const
{
    auto leftVal = left.rect.size.x + left.rect.size.y;
    auto rightVal = right.rect.size.x + right.rect.size.y;

    if(leftVal != rightVal)
        return leftVal < rightVal;

    return left.index < right.index;
}

bool RectPacker::RectInfo::operator < (const RectInfo &right) const
{
    return this->operator () (*this, right);
}

RectPacker::RectPacker(int size)
    : m_size{size}
{
    if(m_size < 0)
        throw Exception{"Rect packer size can't be negative."};
}

void RectPacker::add(int w, int h)
{
    if(w < 0 || h < 0)
        throw Exception{"Rect added to rect packer can't have negative dimensions."};

    auto index = m_rects.size();
    m_rects.emplace_back(index, IntRect{0, 0, w, h});
}

const IntRect &RectPacker::get(int index) const
{
    for(const auto &elem : m_rects) {
        if(elem.index == index)
            return elem.rect;
    }

    throw Exception{"Could not get rect with index " + std::to_string(index) + '.'};
}

std::vector <RectPacker::RectInfo> RectPacker::getAll() const
{
    TRACK;

    auto ret = m_rects;
    std::sort(ret.begin(), ret.end(), sortByIndex);

    return ret;
}

bool RectPacker::pack()
{
    TRACK;

    int zoneIndex{};

    m_freeZones.clear();
    m_freeZones.insert(RectInfo{zoneIndex, IntRect{0, 0, m_size, m_size}});
    ++zoneIndex;

    std::sort(m_rects.begin(), m_rects.end());

    for(auto &elem : m_rects) {
        auto zone = m_freeZones.end();

        for(auto it = m_freeZones.begin(); it != m_freeZones.end(); ++it) {
            if(it->rect.size.x >= elem.rect.size.x && it->rect.size.y >= elem.rect.size.y) {
                zone = it;
                break;
            }
        }

        if(zone == m_freeZones.end())
            return false;

        auto zoneCopy = *zone;

        elem.rect.pos.x = zoneCopy.rect.pos.x;
        elem.rect.pos.y = zoneCopy.rect.pos.y;

        m_freeZones.erase(zone);

        m_freeZones.insert(RectInfo{zoneIndex,
                                    IntRect{zoneCopy.rect.pos.x + elem.rect.size.x,
                                            zoneCopy.rect.pos.y,
                                            zoneCopy.rect.size.x - elem.rect.size.x,
                                            elem.rect.size.y}});

        ++zoneIndex;

        m_freeZones.insert(RectInfo{zoneIndex,
                                    IntRect{zoneCopy.rect.pos.x,
                                            zoneCopy.rect.pos.y + elem.rect.size.y,
                                            zoneCopy.rect.size.x,
                                            zoneCopy.rect.size.y - elem.rect.size.y}});

        ++zoneIndex;
    }

    return true;
}

bool RectPacker::sortByIndex(const RectInfo &left, const RectInfo &right)
{
    return left.index < right.index;
}

} // namespace engine
