#include "WorldPartMob.hpp"

#include "../../Global.hpp"
#include "../../Core.hpp"
#include "../CharacterDef.hpp"
#include "../FactionDef.hpp"
#include "../ItemDef.hpp"
#include "engine/util/DefDatabase.hpp"

namespace app
{

WorldPartMob::WorldPartMob()
    : m_spawnRateWeight{}
{
}

void WorldPartMob::expose(engine::DataFile::Node &node)
{
    node.var(m_characterDef_defName, "characterDef");
    node.var(m_factionDef_defName, "factionDef");
    node.var(m_inHandsItemDef_defName, "inHandsItemDef");
    node.var(m_spawnRateWeight, "spawnRateWeight");

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        auto &defDatabase = Global::getCore().getDefDatabase();

        m_characterDef = defDatabase.getDef <CharacterDef> (m_characterDef_defName);
        m_factionDef = defDatabase.getDef <FactionDef> (m_factionDef_defName);
        m_inHandsItemDef = defDatabase.getDef <ItemDef> (m_inHandsItemDef_defName);

        if(m_spawnRateWeight < 0.f)
            throw engine::Exception{"Spawn rate weight is negative."};
    }
}

const std::shared_ptr <CharacterDef> &WorldPartMob::getCharacterDefPtr() const
{
    if(!m_characterDef)
        throw engine::Exception{"Character def is nullptr."};

    return m_characterDef;
}

const std::shared_ptr <FactionDef> &WorldPartMob::getFactionDefPtr() const
{
    if(!m_factionDef)
        throw engine::Exception{"Faction def is nullptr."};

    return m_factionDef;
}

const std::shared_ptr <ItemDef> &WorldPartMob::getInHandsItemDefPtr() const
{
    if(!m_inHandsItemDef)
        throw engine::Exception{"Item def is nullptr."};

    return m_inHandsItemDef;
}

float WorldPartMob::getSpawnRateWeight() const
{
    return m_spawnRateWeight;
}

} // namespace app
