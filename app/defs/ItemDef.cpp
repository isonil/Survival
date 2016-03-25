#include "ItemDef.hpp"

#include "engine/app3D/defs/SoundDef.hpp"
#include "engine/app3D/defs/ModelDef.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/GUI/IGUITexture.hpp"
#include "engine/GUI/IGUIRenderer.hpp"
#include "engine/GUI/GUIManager.hpp"
#include "engine/util/DefDatabase.hpp"
#include "../Global.hpp"
#include "../Core.hpp"

namespace app
{

ItemDef::ItemDef()
    : m_maxStack{1},
      m_slotType{SingleSlotItemContainer::SlotType::None}
{
}

void ItemDef::expose(engine::DataFile::Node &node)
{
    TRACK;

    base::expose(node);

    node.var(m_modelDef_defName, "modelDef");
    node.var(m_reloadSoundDef_defName, "reloadSoundDef", {});
    node.var(m_equipSoundDef_defName, "equipSoundDef", {});
    node.var(m_putAwaySoundDef_defName, "putAwaySoundDef", {});
    node.var(m_slotType, "slotType", SingleSlotItemContainer::SlotType::None);
    node.var(m_textureInInventory_path, "textureInInventory");
    node.var(m_sizeInInventory, "sizeInInventory", {1, 1});
    node.var(m_maxStack, "maxStack", 1);
    node.var(m_FPPProperties, "FPPProperties", {});
    node.var(m_onUsed, "onUsed", {});
    node.var(m_characterStatsChange, "characterStatsChange", {});

    if(node.getActivityType() == engine::DataFile::Activity::Type::Loading) {
        auto &core = Global::getCore();
        auto &defDatabase = core.getDefDatabase();
        auto &GUIManager = core.getDevice().getGUIManager();

        m_modelDef = defDatabase.getDef <engine::app3D::ModelDef> (m_modelDef_defName);

        if(!m_reloadSoundDef_defName.empty())
            m_reloadSoundDef = defDatabase.getDef <engine::app3D::SoundDef> (m_reloadSoundDef_defName);
        else
            m_reloadSoundDef.reset();

        if(!m_equipSoundDef_defName.empty())
            m_equipSoundDef = defDatabase.getDef <engine::app3D::SoundDef> (m_equipSoundDef_defName);
        else
            m_equipSoundDef.reset();

        if(!m_putAwaySoundDef_defName.empty())
            m_putAwaySoundDef = defDatabase.getDef <engine::app3D::SoundDef> (m_putAwaySoundDef_defName);
        else
            m_putAwaySoundDef.reset();

        m_textureInInventory = GUIManager.getRenderer().getTexture(m_textureInInventory_path);

        if(m_sizeInInventory.x < 0 || m_sizeInInventory.y < 0)
            throw engine::Exception{"Size in inventory can't be negative."};

        if(m_maxStack <= 0)
            throw engine::Exception{"Max stack can't be <= 0."};
    }
}

const engine::app3D::ModelDef &ItemDef::getModelDef() const
{
    if(!m_modelDef)
        throw engine::Exception{"Model def is nullptr."};

    return *m_modelDef;
}

const std::shared_ptr <engine::app3D::ModelDef> &ItemDef::getModelDefPtr() const
{
    if(!m_modelDef)
        throw engine::Exception{"Model def is nullptr."};

    return m_modelDef;
}

bool ItemDef::hasReloadSoundDef() const
{
    return static_cast <bool> (m_reloadSoundDef);
}

const std::shared_ptr <engine::app3D::SoundDef> &ItemDef::getReloadSoundDefPtr() const
{
    if(!m_reloadSoundDef)
        throw engine::Exception{"Reload sound def is nullptr. This should have been checked before."};

    return m_reloadSoundDef;
}

bool ItemDef::hasEquipSoundDef() const
{
    return static_cast <bool> (m_equipSoundDef);
}

const std::shared_ptr <engine::app3D::SoundDef> &ItemDef::getEquipSoundDefPtr() const
{
    if(!m_equipSoundDef)
        throw engine::Exception{"Equip sound def is nullptr. This should have been checked before."};

    return m_equipSoundDef;
}

bool ItemDef::hasPutAwaySoundDef() const
{
    return static_cast <bool> (m_putAwaySoundDef);
}

const std::shared_ptr <engine::app3D::SoundDef> &ItemDef::getPutAwaySoundDefPtr() const
{
    if(!m_putAwaySoundDef)
        throw engine::Exception{"Put away sound def is nullptr. This should have been checked before."};

    return m_putAwaySoundDef;
}

const SingleSlotItemContainer::SlotType &ItemDef::getSlotType() const
{
    return m_slotType;
}

const engine::GUI::IGUITexture &ItemDef::getTextureInInventory() const
{
    if(!m_textureInInventory)
        throw engine::Exception{"Texture in inventory is nullptr."};

    return *m_textureInInventory;
}

const std::shared_ptr <engine::GUI::IGUITexture> &ItemDef::getTextureInInventoryPtr() const
{
    if(!m_textureInInventory)
        throw engine::Exception{"Texture in inventory is nullptr."};

    return m_textureInInventory;
}

const engine::IntVec2 &ItemDef::getSizeInInventory() const
{
    return m_sizeInInventory;
}

int ItemDef::getMaxStack() const
{
    return m_maxStack;
}

const ItemFPPProperties &ItemDef::getFPPProperties() const
{
    return m_FPPProperties;
}

const OnUsedItem &ItemDef::getOnUsed() const
{
    return m_onUsed;
}

CharacterStatsChange &ItemDef::getCharacterStatsChange()
{
    return m_characterStatsChange;
}

const CharacterStatsChange &ItemDef::getCharacterStatsChange() const
{
    return m_characterStatsChange;
}

const float ItemDef::k_meleeRange{2.f};
const float ItemDef::k_nonMeleeRange{500.f};

} // namespace app
