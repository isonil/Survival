#ifndef ENGINE_QUAD_TREE_HPP
#define ENGINE_QUAD_TREE_HPP

#include "Trace.hpp"
#include "Rect.hpp"
#include "Vec2.hpp"

#include <memory>

namespace engine
{

class QuadTree
{
public:
    explicit QuadTree(const IntRect &rect);
    QuadTree(const QuadTree &) = delete;

    QuadTree &operator = (const QuadTree &) = delete;

    bool insert(const IntVec2 &vec);
    std::vector <IntVec2> query(const IntRect &rect) const;

private:
    void subdivide();

    static const size_t k_nodeCapacity;

    IntRect m_rect;
    std::vector <IntVec2> m_points;
    std::unique_ptr <QuadTree> m_northWest;
    std::unique_ptr <QuadTree> m_northEast;
    std::unique_ptr <QuadTree> m_southWest;
    std::unique_ptr <QuadTree> m_southEast;
};

} // namespace engine

#endif // ENGINE_QUAD_TREE_HPP
