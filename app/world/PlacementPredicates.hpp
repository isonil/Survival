#ifndef APP_PLACEMENT_PREDICATES_HPP
#define APP_PLACEMENT_PREDICATES_HPP

#include "engine/util/DataFile.hpp"
#include "engine/util/Range.hpp"

namespace app
{

class PlacementPredicates : public engine::DataFile::Saveable
{
public:
    PlacementPredicates();
    PlacementPredicates(const engine::FloatRange &slopeRange, bool onlyAboveWaterLevel, bool onlyBelowWaterLevel);

    void expose(engine::DataFile::Node &node) override;

    const engine::FloatRange &getSlopeRange() const;
    bool getOnlyAboveWaterLevel() const;
    bool getOnlyBelowWaterLevel() const;
    bool isAllowedGround1() const;
    bool isAllowedGround2() const;
    bool isAllowedGround3() const;

private:
    void validateAndFix();

    engine::FloatRange m_slopeRange;
    bool m_onlyAboveWaterLevel;
    bool m_onlyBelowWaterLevel;
    bool m_isAllowedGround1;
    bool m_isAllowedGround2;
    bool m_isAllowedGround3;
};

} // namespace app

#endif // APP_PLACEMENT_PREDICATES_HPP
