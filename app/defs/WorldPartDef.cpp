#include "WorldPartDef.hpp"

#include "engine/app3D/defs/SoundDef.hpp"
#include "engine/app3D/defs/TerrainDef.hpp"
#include "engine/util/DefDatabase.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "EntityDef.hpp"
#include "FactionDef.hpp"

namespace app
{

void WorldPartDef::PrespawnedEntity::Params::expose(engine::DataFile::Node &node)
{
    node.var(m_searchableItemContainerItems, "searchableItemContainerItems");
}

const ItemsList &WorldPartDef::PrespawnedEntity::Params::getSearchableItemContainerItems() const
{
    return m_searchableItemContainerItems;
}

WorldPartDef::PrespawnedEntity::PrespawnedEntity()
    : m_adjustPositionToTerrain{}
{
}

void WorldPartDef::PrespawnedEntity::expose(engine::DataFile::Node &node)
{
    node.var(m_entityDef_defName, "def");
    node.var(m_factionDef_defName, "factionDef");
    node.var(m_pos, "position");
    node.var(m_rot, "rotation", {});
    node.var(m_adjustPositionToTerrain, "adjustPositionToTerrain", {});
    node.var(m_params, "params", {});

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        auto &defDatabase = Global::getCore().getDefDatabase();

        m_entityDef = defDatabase.getDef <EntityDef> (m_entityDef_defName);
        m_factionDef = defDatabase.getDef <FactionDef> (m_factionDef_defName);
    }
}

const std::shared_ptr <EntityDef> &WorldPartDef::PrespawnedEntity::getEntityDefPtr() const
{
    if(!m_entityDef)
        throw engine::Exception{"Entity def is nullptr."};

    return m_entityDef;
}

const std::shared_ptr <FactionDef> &WorldPartDef::PrespawnedEntity::getFactionDefPtr() const
{
    if(!m_factionDef)
        throw engine::Exception{"Faction def is nullptr."};

    return m_factionDef;
}

const engine::FloatVec3 &WorldPartDef::PrespawnedEntity::getPosition() const
{
    return m_pos;
}

const engine::FloatVec3 &WorldPartDef::PrespawnedEntity::getRotation() const
{
    return m_rot;
}

bool WorldPartDef::PrespawnedEntity::shouldAdjustPositionToTerrain() const
{
    return m_adjustPositionToTerrain;
}

const WorldPartDef::PrespawnedEntity::Params &WorldPartDef::PrespawnedEntity::getParams() const
{
    return m_params;
}

WorldPartDef::WorldPartDef()
    : m_isPlayerStartingWorldPart{},
      m_randomMineablesCount{}
{
}

void WorldPartDef::expose(engine::DataFile::Node &node)
{
    TRACK;

    base::expose(node);

    node.var(m_terrainDef_defName, "terrainDef");
    node.var(m_ground1StepSoundDef_defName, "ground1StepSoundDef");
    node.var(m_ground2StepSoundDef_defName, "ground2StepSoundDef");
    node.var(m_ground3StepSoundDef_defName, "ground3StepSoundDef");
    node.var(m_slopeStepSoundDef_defName, "slopeStepSoundDef");
    node.var(m_isPlayerStartingWorldPart, "isPlayerStartingWorldPart", {});
    node.var(m_playerStartingPosition, "playerStartingPosition", {});
    node.var(m_randomMineablesCount, "randomMineablesCount");
    node.var(m_randomMineables, "randomMineables");
    node.var(m_mobs, "mobs");
    node.var(m_prespawnedEntities, "prespawnedEntities");

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        auto &defDatabase = Global::getCore().getDefDatabase();

        m_terrainDef = defDatabase.getDef <engine::app3D::TerrainDef> (m_terrainDef_defName);

        m_ground1StepSoundDef = defDatabase.getDef <engine::app3D::SoundDef> (m_ground1StepSoundDef_defName);
        m_ground2StepSoundDef = defDatabase.getDef <engine::app3D::SoundDef> (m_ground2StepSoundDef_defName);
        m_ground3StepSoundDef = defDatabase.getDef <engine::app3D::SoundDef> (m_ground3StepSoundDef_defName);
        m_slopeStepSoundDef = defDatabase.getDef <engine::app3D::SoundDef> (m_slopeStepSoundDef_defName);

        if(m_randomMineablesCount < 0)
            throw engine::Exception{"Random mineables count is negative."};
    }
}

const engine::app3D::TerrainDef &WorldPartDef::getTerrainDef() const
{
    if(!m_terrainDef)
        throw engine::Exception{"Terrain def is nullptr."};

    return *m_terrainDef;
}

const std::shared_ptr <engine::app3D::TerrainDef> &WorldPartDef::getTerrainDefPtr() const
{
    if(!m_terrainDef)
        throw engine::Exception{"Terrain def is nullptr."};

    return m_terrainDef;
}

const std::shared_ptr <engine::app3D::SoundDef> &WorldPartDef::getGround1StepSoundDefPtr() const
{
    if(!m_ground1StepSoundDef)
        throw engine::Exception{"Ground 1 step sound def is nullptr."};

    return m_ground1StepSoundDef;
}

const std::shared_ptr <engine::app3D::SoundDef> &WorldPartDef::getGround2StepSoundDefPtr() const
{
    if(!m_ground2StepSoundDef)
        throw engine::Exception{"Ground 2 step sound def is nullptr."};

    return m_ground2StepSoundDef;
}

const std::shared_ptr <engine::app3D::SoundDef> &WorldPartDef::getGround3StepSoundDefPtr() const
{
    if(!m_ground3StepSoundDef)
        throw engine::Exception{"Ground 3 step sound def is nullptr."};

    return m_ground3StepSoundDef;
}

const std::shared_ptr <engine::app3D::SoundDef> &WorldPartDef::getSlopeStepSoundDefPtr() const
{
    if(!m_slopeStepSoundDef)
        throw engine::Exception{"Slope step sound def is nullptr."};

    return m_slopeStepSoundDef;
}

bool WorldPartDef::isPlayerStartingWorldPart() const
{
    return m_isPlayerStartingWorldPart;
}

const engine::FloatVec2 &WorldPartDef::getPlayerStartingPosition() const
{
    if(!isPlayerStartingWorldPart())
        throw engine::Exception{"Tried to get player starting position from WorldPart which is not player starting world part. This should have been checked before."};

    return m_playerStartingPosition;
}

int WorldPartDef::getRandomMineablesCount() const
{
    return m_randomMineablesCount;
}

const std::vector <WorldPartRandomMineable> &WorldPartDef::getRandomMineables() const
{
    return m_randomMineables;
}

const std::vector <WorldPartMob> &WorldPartDef::getMobs() const
{
    return m_mobs;
}

const std::vector <WorldPartDef::PrespawnedEntity> &WorldPartDef::getPrespawnedEntities() const
{
    return m_prespawnedEntities;
}

} // namespace app
