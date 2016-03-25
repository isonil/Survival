#include "CollisionFilter.hpp"

namespace engine
{
namespace app3D
{

CollisionFilter operator | (const CollisionFilter &lhs, const CollisionFilter &rhs)
{
    return static_cast <CollisionFilter> (static_cast <short int> (lhs) | static_cast <short int> (rhs));
}

CollisionFilter operator & (const CollisionFilter &lhs, const CollisionFilter &rhs)
{
    return static_cast <CollisionFilter> (static_cast <short int> (lhs) & static_cast <short int> (rhs));
}

CollisionFilter operator ^ (const CollisionFilter &lhs, const CollisionFilter &rhs)
{
    return static_cast <CollisionFilter> (static_cast <short int> (lhs) ^ static_cast <short int> (rhs));
}

} // namespace app3D
} // namespace engine
