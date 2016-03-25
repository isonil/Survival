#include "QuadTree.hpp"

#include "Exception.hpp"

namespace engine
{

QuadTree::QuadTree(const IntRect &rect)
    : m_rect{rect},
      m_northWest{},
      m_northEast{},
      m_southWest{},
      m_southEast{}
{
    if(m_rect.size.x <= 0)
        m_rect.size.x = 1;

    if(m_rect.size.y <= 0)
        m_rect.size.y = 1;

    m_rect.size.squareComplement();
    m_rect.size.POTComplement();
}

bool QuadTree::insert(const IntVec2 &vec)
{
    TRACK;

    if(!m_rect.contains(vec))
        return false;

    if(m_points.size() < k_nodeCapacity || m_rect.size.x <= 1 || m_rect.size.y <= 1) {
        m_points.push_back(vec);
        return true;
    }

    if(!m_northWest || !m_northEast || !m_southWest || !m_southEast) {
        subdivide();
    }

    if(m_northWest->insert(vec)) return true;
    if(m_northEast->insert(vec)) return true;
    if(m_southWest->insert(vec)) return true;
    if(m_southEast->insert(vec)) return true;

    throw Exception{"Could not insert point (" + std::to_string(vec.x) + ' ' + std::to_string(vec.y) +") to QuadTree."};
}

std::vector <IntVec2> QuadTree::query(const IntRect &rect) const
{
    TRACK;

    std::vector <IntVec2> points;

    if(!m_rect.intersects(rect))
        return points;

    for(const auto &elem : m_points) {
        if(rect.contains(elem)) {
            points.push_back(elem);
        }
    }

    if(!m_northWest || !m_northEast || !m_southWest || !m_southEast) {
        return points;
    }

    std::vector <IntVec2> pts1, pts2, pts3, pts4;

    pts1 = m_northWest->query(rect);
    pts2 = m_northEast->query(rect);
    pts3 = m_southWest->query(rect);
    pts4 = m_southEast->query(rect);

    for(const auto &elem : pts1) {
        points.push_back(elem);
    }
    for(const auto &elem : pts2) {
        points.push_back(elem);
    }
    for(const auto &elem : pts3) {
        points.push_back(elem);
    }
    for(const auto &elem : pts4) {
        points.push_back(elem);
    }

    return points;
}

void QuadTree::subdivide()
{
    TRACK;

    int halfW{m_rect.size.x / 2};
    int halfH{m_rect.size.y / 2};

    if(halfW <= 0 || halfH <= 0)
        throw Exception{"Could not subdivide QuadTree because half size is <= 0."};

    m_northWest = std::make_unique <QuadTree> (IntRect{m_rect.pos.x, m_rect.pos.y, halfW, halfH});
    m_northEast = std::make_unique <QuadTree> (IntRect{m_rect.pos.x + halfW, m_rect.pos.y, halfW, halfH});
    m_southWest = std::make_unique <QuadTree> (IntRect{m_rect.pos.x, m_rect.pos.y + halfH, halfW, halfH});
    m_southEast = std::make_unique <QuadTree> (IntRect{m_rect.pos.x + halfW, m_rect.pos.y + halfH, halfW, halfH});
}

const size_t QuadTree::k_nodeCapacity{4};

} // namespace engine
