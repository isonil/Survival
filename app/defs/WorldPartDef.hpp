#ifndef APP_WORLD_PART_DEF_HPP
#define APP_WORLD_PART_DEF_HPP

#include "engine/util/Vec2.hpp"
#include "engine/util/Vec3.hpp"
#include "engine/util/Trace.hpp"
#include "engine/util/Def.hpp"
#include "parts/WorldPartRandomMineable.hpp"
#include "parts/WorldPartMob.hpp"
#include "parts/ItemsList.hpp"

#include <memory>

namespace engine { namespace app3D { class TerrainDef; class SoundDef; } }

namespace app
{

class EntityDef;

class WorldPartDef : public engine::Def, public engine::Tracked <WorldPartDef>
{
public:
    class PrespawnedEntity : public engine::DataFile::Saveable
    {
    public:
        class Params : public engine::DataFile::Saveable
        {
        public:
            void expose(engine::DataFile::Node &node) override;

            const ItemsList &getSearchableItemContainerItems() const;

        private:
            ItemsList m_searchableItemContainerItems;
        };

        PrespawnedEntity();

        void expose(engine::DataFile::Node &node) override;

        const std::shared_ptr <EntityDef> &getEntityDefPtr() const;
        const std::shared_ptr <FactionDef> &getFactionDefPtr() const;
        const engine::FloatVec3 &getPosition() const;
        const engine::FloatVec3 &getRotation() const;
        bool shouldAdjustPositionToTerrain() const;
        const Params &getParams() const;

    private:
        std::string m_entityDef_defName;
        std::shared_ptr <EntityDef> m_entityDef;
        std::string m_factionDef_defName;
        std::shared_ptr <FactionDef> m_factionDef;
        engine::FloatVec3 m_pos;
        engine::FloatVec3 m_rot;
        bool m_adjustPositionToTerrain;
        Params m_params;
    };

    WorldPartDef();

    void expose(engine::DataFile::Node &node) override;

    const engine::app3D::TerrainDef &getTerrainDef() const;
    const std::shared_ptr <engine::app3D::TerrainDef> &getTerrainDefPtr() const;
    const std::shared_ptr <engine::app3D::SoundDef> &getGround1StepSoundDefPtr() const;
    const std::shared_ptr <engine::app3D::SoundDef> &getGround2StepSoundDefPtr() const;
    const std::shared_ptr <engine::app3D::SoundDef> &getGround3StepSoundDefPtr() const;
    const std::shared_ptr <engine::app3D::SoundDef> &getSlopeStepSoundDefPtr() const;
    bool isPlayerStartingWorldPart() const;
    const engine::FloatVec2 &getPlayerStartingPosition() const;
    int getRandomMineablesCount() const;
    const std::vector <WorldPartRandomMineable> &getRandomMineables() const;
    const std::vector <WorldPartMob> &getMobs() const;
    const std::vector <PrespawnedEntity> &getPrespawnedEntities() const;

private:
    using base = Def;

    std::string m_terrainDef_defName;
    std::shared_ptr <engine::app3D::TerrainDef> m_terrainDef;
    std::string m_ground1StepSoundDef_defName;
    std::shared_ptr <engine::app3D::SoundDef> m_ground1StepSoundDef;
    std::string m_ground2StepSoundDef_defName;
    std::shared_ptr <engine::app3D::SoundDef> m_ground2StepSoundDef;
    std::string m_ground3StepSoundDef_defName;
    std::shared_ptr <engine::app3D::SoundDef> m_ground3StepSoundDef;
    std::string m_slopeStepSoundDef_defName;
    std::shared_ptr <engine::app3D::SoundDef> m_slopeStepSoundDef;
    bool m_isPlayerStartingWorldPart;
    engine::FloatVec2 m_playerStartingPosition;
    int m_randomMineablesCount;
    std::vector <WorldPartRandomMineable> m_randomMineables;
    std::vector <WorldPartMob> m_mobs;
    std::vector <PrespawnedEntity> m_prespawnedEntities;
};

} // namespace app

#endif // APP_WORLD_PART_DEF_HPP
