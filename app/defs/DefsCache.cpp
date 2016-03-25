#include "DefsCache.hpp"

#include "engine/app3D/defs/ModelDef.hpp"
#include "engine/app3D/defs/SoundDef.hpp"
#include "engine/app3D/defs/LightDef.hpp"
#include "engine/util/DefDatabase.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "CharacterDef.hpp"
#include "CachedCollisionShapeDef.hpp"
#include "FactionDef.hpp"
#include "UpgradeDef.hpp"
#include "EffectDef.hpp"
#include "WorldPartDef.hpp"
#include "CraftingRecipeDef.hpp"

namespace app
{

DefsCache::DefsCache()
{
    init();
}

void DefsCache::init()
{
    TRACK;

    auto &defDatabase = Global::getCore().getDefDatabase();

    Character_Human = defDatabase.getDef <CharacterDef> ("Character_Human");

    CachedCollisionShape_Plane_0 = defDatabase.getDef <CachedCollisionShapeDef> ("CachedCollisionShape_Plane_0");
    CachedCollisionShape_ItemPickUpZone = defDatabase.getDef <CachedCollisionShapeDef> ("CachedCollisionShape_ItemPickUpZone");

    Model_ElectricitySymbol_Normal_Normal = defDatabase.getDef <engine::app3D::ModelDef> ("Model_ElectricitySymbol_Normal_Normal");
    Model_ElectricitySymbol_Generator_Normal = defDatabase.getDef <engine::app3D::ModelDef> ("Model_ElectricitySymbol_Generator_Normal");
    Model_ElectricitySymbol_Normal_Gray = defDatabase.getDef <engine::app3D::ModelDef> ("Model_ElectricitySymbol_Normal_Gray");
    Model_ElectricitySymbol_Generator_Gray = defDatabase.getDef <engine::app3D::ModelDef> ("Model_ElectricitySymbol_Generator_Gray");
    Model_ElectricitySymbol_Normal_Red = defDatabase.getDef <engine::app3D::ModelDef> ("Model_ElectricitySymbol_Normal_Red");
    Model_ElectricitySymbol_Generator_Red = defDatabase.getDef <engine::app3D::ModelDef> ("Model_ElectricitySymbol_Generator_Red");

    Sound_Poof = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_Poof");
    Sound_WaterStep = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_WaterStep");
    Sound_Constructed = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_Constructed");
    Sound_StartSwimming = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_StartSwimming");
    Sound_LeaveWater = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_LeaveWater");
    Sound_FallIntoWater = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_FallIntoWater");
    Sound_Swim1 = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_Swim1");
    Sound_Swim2 = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_Swim2");
    Sound_TargetAcquired = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_TargetAcquired");
    Sound_TurretHeadMove = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_TurretHeadMove");

    Sound_StartAiming = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_StartAiming");
    Sound_StopAiming = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_StopAiming");
    Sound_Inventory1 = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_Inventory1");
    Sound_Inventory2 = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_Inventory2");
    Sound_EquipWeapon = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_EquipWeapon");
    Sound_Select1 = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_Select1");
    Sound_Select2 = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_Select2");
    Sound_Select3 = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_Select3");
    Sound_Switch1 = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_Switch1");
    Sound_Negative1 = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_Negative1");
    Sound_GoUnderWater = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_GoUnderWater");
    Sound_LeaveFromUnderWater = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_LeaveFromUnderWater");
    Sound_HitTarget = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_HitTarget");
    Sound_ITakeDamage = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_ITakeDamage");
    Sound_MyDeath = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_MyDeath");
    Sound_UnlockUpgrade = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_UnlockUpgrade");
    Sound_LevelUp = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_LevelUp");
    Sound_SearchItemContainer = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_SearchItemContainer");
    Sound_Crafted = defDatabase.getDef <engine::app3D::SoundDef> ("Sound_Crafted");

    Light_Directional = defDatabase.getDef <engine::app3D::LightDef> ("Light_Directional");

    Faction_Neutral = defDatabase.getDef <FactionDef> ("Faction_Neutral");
    Faction_Players = defDatabase.getDef <FactionDef> ("Faction_Players");

    Effect_Poof = defDatabase.getDef <EffectDef> ("Effect_Poof");
    Effect_SmallExplosion = defDatabase.getDef <EffectDef> ("Effect_SmallExplosion");
    Effect_Explosion = defDatabase.getDef <EffectDef> ("Effect_Explosion");
    Effect_BigExplosion = defDatabase.getDef <EffectDef> ("Effect_BigExplosion");
    Effect_OnHitGeneric = defDatabase.getDef <EffectDef> ("Effect_OnHitGeneric");

    WorldPart_Water = defDatabase.getDef <WorldPartDef> ("WorldPart_Water");

    AllFactionRelations.clear();
    for(const auto &elem : defDatabase.getDefs <FactionRelationDef> ()) {
        AllFactionRelations.push_back(elem.second);
    }

    AllUpgrades.clear();
    for(const auto &elem : defDatabase.getDefs <UpgradeDef> ()) {
        AllUpgrades.push_back(elem.second);
    }

    AllCraftingRecipes.clear();
    for(const auto &elem : defDatabase.getDefs <CraftingRecipeDef> ()) {
        AllCraftingRecipes.push_back(elem.second);
    }
}

} // namespace app
