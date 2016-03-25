#ifndef APP_ITEM_DEF_HPP
#define APP_ITEM_DEF_HPP

#include "../entities/character/CharacterStatsChange.hpp"
#include "../itemContainers/SingleSlotItemContainer.hpp"
#include "engine/util/Vec2.hpp"
#include "engine/util/Vec3.hpp"
#include "engine/util/Enum.hpp"
#include "parts/ItemFPPProperties.hpp"
#include "parts/OnUsedItem.hpp"
#include "EntityDef.hpp"

#include <memory>

namespace engine
{

namespace app3D { class ModelDef; }
namespace GUI { class IGUITexture; }

} // namespace engine

namespace app
{

class ItemDef : public EntityDef, public engine::Tracked <ItemDef>
{
public:
    ItemDef();

    void expose(engine::DataFile::Node &node) override;

    const engine::app3D::ModelDef &getModelDef() const;
    const std::shared_ptr <engine::app3D::ModelDef> &getModelDefPtr() const;

    bool hasReloadSoundDef() const;
    const std::shared_ptr <engine::app3D::SoundDef> &getReloadSoundDefPtr() const;

    bool hasEquipSoundDef() const;
    const std::shared_ptr <engine::app3D::SoundDef> &getEquipSoundDefPtr() const;

    bool hasPutAwaySoundDef() const;
    const std::shared_ptr <engine::app3D::SoundDef> &getPutAwaySoundDefPtr() const;

    const SingleSlotItemContainer::SlotType &getSlotType() const;
    const engine::GUI::IGUITexture &getTextureInInventory() const;
    const std::shared_ptr <engine::GUI::IGUITexture> &getTextureInInventoryPtr() const;
    const engine::IntVec2 &getSizeInInventory() const;
    int getMaxStack() const;
    const ItemFPPProperties &getFPPProperties() const;
    const OnUsedItem &getOnUsed() const;
    CharacterStatsChange &getCharacterStatsChange();
    const CharacterStatsChange &getCharacterStatsChange() const;

    static const float k_meleeRange;
    static const float k_nonMeleeRange;

private:
    using base = EntityDef;

    int m_maxStack;
    std::string m_modelDef_defName;
    std::shared_ptr <engine::app3D::ModelDef> m_modelDef;

    std::string m_reloadSoundDef_defName;
    std::shared_ptr <engine::app3D::SoundDef> m_reloadSoundDef;

    std::string m_equipSoundDef_defName;
    std::shared_ptr <engine::app3D::SoundDef> m_equipSoundDef;

    std::string m_putAwaySoundDef_defName;
    std::shared_ptr <engine::app3D::SoundDef> m_putAwaySoundDef;

    SingleSlotItemContainer::SlotType m_slotType;
    std::string m_textureInInventory_path;
    std::shared_ptr <engine::GUI::IGUITexture> m_textureInInventory;
    engine::IntVec2 m_sizeInInventory;
    ItemFPPProperties m_FPPProperties;
    OnUsedItem m_onUsed;
    CharacterStatsChange m_characterStatsChange;
};

} // namespace app

#endif // APP_ITEM_DEF_HPP
