#ifndef APP_WORLD_PART_RANDOM_MINEABLE_HPP
#define APP_WORLD_PART_RANDOM_MINEABLE_HPP

#include "../../world/PlacementPredicates.hpp"
#include "engine/util/DataFile.hpp"

namespace app
{

class MineableDef;

class WorldPartRandomMineable : public engine::DataFile::Saveable, public engine::Tracked <WorldPartRandomMineable>
{
public:
    WorldPartRandomMineable();

    void expose(engine::DataFile::Node &node) override;

    const MineableDef &getMineableDef() const;
    const std::shared_ptr <MineableDef> &getMineableDefPtr() const;
    float getDensity() const;
    const PlacementPredicates &getPlacementPredicates() const;

private:
    float m_density;
    std::string m_mineableDef_defName;
    std::shared_ptr <MineableDef> m_mineableDef;
    PlacementPredicates m_placementPredicates;
};

} // namespace app

#endif // APP_WORLD_PART_RANDOM_MINEABLE_HPP
