#include "PlacementPredicates.hpp"

namespace app
{

PlacementPredicates::PlacementPredicates()
    : m_slopeRange{0.f, 1.f},
      m_onlyAboveWaterLevel{},
      m_onlyBelowWaterLevel{},
      m_isAllowedGround1{true},
      m_isAllowedGround2{true},
      m_isAllowedGround3{true}
{
}

PlacementPredicates::PlacementPredicates(const engine::FloatRange &slopeRange, bool onlyAboveWaterLevel, bool onlyBelowWaterLevel)
    : m_slopeRange{slopeRange},
      m_onlyAboveWaterLevel{onlyAboveWaterLevel},
      m_onlyBelowWaterLevel{onlyBelowWaterLevel},
      m_isAllowedGround1{true},
      m_isAllowedGround2{true},
      m_isAllowedGround3{true}
{
    validateAndFix();
}

void PlacementPredicates::expose(engine::DataFile::Node &node)
{
    node.var(m_slopeRange, "slopeRange", {0.f, 1.f});
    node.var(m_onlyAboveWaterLevel, "onlyAboveWaterLevel", {});
    node.var(m_onlyBelowWaterLevel, "onlyBelowWaterLevel", {});
    node.var(m_isAllowedGround1, "isAllowedGround1", true);
    node.var(m_isAllowedGround2, "isAllowedGround2", true);
    node.var(m_isAllowedGround3, "isAllowedGround3", true);

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading)
        validateAndFix();
}

const engine::FloatRange &PlacementPredicates::getSlopeRange() const
{
    return m_slopeRange;
}

bool PlacementPredicates::getOnlyAboveWaterLevel() const
{
    return m_onlyAboveWaterLevel;
}

bool PlacementPredicates::getOnlyBelowWaterLevel() const
{
    return m_onlyBelowWaterLevel;
}

bool PlacementPredicates::isAllowedGround1() const
{
    return m_isAllowedGround1;
}

bool PlacementPredicates::isAllowedGround2() const
{
    return m_isAllowedGround2;
}

bool PlacementPredicates::isAllowedGround3() const
{
    return m_isAllowedGround3;
}

void PlacementPredicates::validateAndFix()
{
    if(m_onlyAboveWaterLevel && m_onlyBelowWaterLevel) {
        m_onlyAboveWaterLevel = false;
        m_onlyBelowWaterLevel = false;
    }
}

} // namespace app
