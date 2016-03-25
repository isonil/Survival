#include "WorldPartRandomMineable.hpp"

#include "../../Global.hpp"
#include "../../Core.hpp"
#include "engine/util/DefDatabase.hpp"
#include "../MineableDef.hpp"

namespace app
{

WorldPartRandomMineable::WorldPartRandomMineable()
    : m_density{}
{
}

void WorldPartRandomMineable::expose(engine::DataFile::Node &node)
{
    TRACK;

    node.var(m_mineableDef_defName, "def");
    node.var(m_density, "density");
    node.var(m_placementPredicates, "placementPredicates", {});

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        auto &defDatabase = Global::getCore().getDefDatabase();
        m_mineableDef = defDatabase.getDef <MineableDef> (m_mineableDef_defName);

        if(m_density < 0.f)
            throw engine::Exception{"Mineable density is negative."};
    }
}

const MineableDef &WorldPartRandomMineable::getMineableDef() const
{
    if(!m_mineableDef)
        throw engine::Exception{"Mineable def is nullptr."};

    return *m_mineableDef;
}

const std::shared_ptr <MineableDef> &WorldPartRandomMineable::getMineableDefPtr() const
{
    if(!m_mineableDef)
        throw engine::Exception{"Mineable def is nullptr."};

    return m_mineableDef;
}

float WorldPartRandomMineable::getDensity() const
{
    return m_density;
}

const PlacementPredicates &WorldPartRandomMineable::getPlacementPredicates() const
{
    return m_placementPredicates;
}

} // namespace app
