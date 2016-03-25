#ifndef APP_WORLD_PART_MOB_HPP
#define APP_WORLD_PART_MOB_HPP

#include "engine/util/Trace.hpp"
#include "engine/util/DataFile.hpp"

namespace app
{

class CharacterDef;
class FactionDef;
class ItemDef;

class WorldPartMob : public engine::DataFile::Saveable, public engine::Tracked <WorldPartMob>
{
public:
    WorldPartMob();

    void expose(engine::DataFile::Node &node) override;

    const std::shared_ptr <CharacterDef> &getCharacterDefPtr() const;
    const std::shared_ptr <FactionDef> &getFactionDefPtr() const;
    const std::shared_ptr <ItemDef> &getInHandsItemDefPtr() const;
    float getSpawnRateWeight() const;

private:
    std::string m_characterDef_defName;
    std::shared_ptr <CharacterDef> m_characterDef;
    std::string m_factionDef_defName;
    std::shared_ptr <FactionDef> m_factionDef;
    std::string m_inHandsItemDef_defName;
    std::shared_ptr <ItemDef> m_inHandsItemDef;
    float m_spawnRateWeight;
};

} // namespace app

#endif // APP_WORLD_PART_MOB_HPP

