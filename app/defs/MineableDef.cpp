#include "MineableDef.hpp"

#include "engine/app3D/defs/ModelDef.hpp"
#include "engine/util/DefDatabase.hpp"
#include "engine/util/Random.hpp"
#include "../Global.hpp"
#include "../Core.hpp"

namespace app
{

MineableDef::MineableDef()
    : m_initialDurability{}
{
}

void MineableDef::expose(engine::DataFile::Node &node)
{
    TRACK;

    base::expose(node);

    node.var(m_modelDefs_names, "modelDefs");
    node.var(m_resources, "resources", {});
    node.var(m_itemsToSpawnWhenDestroyed, "itemsToSpawnWhenDestroyed", {});
    node.var(m_mineableTags, "mineableTags");
    node.var(m_initialDurability, "initialDurability", {});

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        m_modelDefs.clear();

        if(m_modelDefs_names.empty())
            throw engine::Exception{"Mineable def \"" + getDefName() + "\" has no model defs."};

        auto &defDatabase = Global::getCore().getDefDatabase();

        for(const auto &elem : m_modelDefs_names) {
            const auto &def = defDatabase.getDef <engine::app3D::ModelDef> (elem);
            m_modelDefs.push_back(def);
        }

        if(m_initialDurability < 0 || m_initialDurability > 100)
            throw engine::Exception{"Initial durability must be between 0 and 100."};
    }
}

const std::shared_ptr <engine::app3D::ModelDef> &MineableDef::getRandomModelDefPtr() const
{
    TRACK;

    if(m_modelDefs.empty())
        throw engine::Exception{"Mineable def \"" + getDefName() + "\" has no model defs."};

    return m_modelDefs[engine::Random::rangeExclusive(0, m_modelDefs.size())];
}

const ItemsListWithUnboundedStack &MineableDef::getResources() const
{
    return m_resources;
}

const ItemsList &MineableDef::getItemsToSpawnWhenDestroyed() const
{
    return m_itemsToSpawnWhenDestroyed;
}

bool MineableDef::hasMineableTag(const std::string &mineableTag) const
{
    return std::find(m_mineableTags.begin(), m_mineableTags.end(), mineableTag) != m_mineableTags.end();
}

int MineableDef::getInitialDurability() const
{
    return m_initialDurability;
}

} // namespace app
