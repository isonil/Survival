#ifndef APP_DEFS_CACHE_HPP
#define APP_DEFS_CACHE_HPP

#include <memory>
#include <vector>

namespace engine { namespace app3D { class ModelDef; class SoundDef; class LightDef; } }

namespace app
{

class CachedCollisionShapeDef;
class CharacterDef;
class FactionDef;
class FactionRelationDef;
class UpgradeDef;
class EffectDef;
class WorldPartDef;
class CraftingRecipeDef;

// all single defs (not the ones in containers) in DefsCache must come from the core mod
// (because using only core mod must be sufficient to run the game)

struct DefsCache
{
    DefsCache();

    // characters
    std::shared_ptr <CharacterDef> Character_Human;

    // cached collision shapes
    std::shared_ptr <CachedCollisionShapeDef> CachedCollisionShape_Plane_0;
    std::shared_ptr <CachedCollisionShapeDef> CachedCollisionShape_ItemPickUpZone;

    // models
    std::shared_ptr <engine::app3D::ModelDef> Model_ElectricitySymbol_Normal_Normal;
    std::shared_ptr <engine::app3D::ModelDef> Model_ElectricitySymbol_Generator_Normal;
    std::shared_ptr <engine::app3D::ModelDef> Model_ElectricitySymbol_Normal_Gray;
    std::shared_ptr <engine::app3D::ModelDef> Model_ElectricitySymbol_Generator_Gray;
    std::shared_ptr <engine::app3D::ModelDef> Model_ElectricitySymbol_Normal_Red;
    std::shared_ptr <engine::app3D::ModelDef> Model_ElectricitySymbol_Generator_Red;

    // sounds
    std::shared_ptr <engine::app3D::SoundDef> Sound_Poof;
    std::shared_ptr <engine::app3D::SoundDef> Sound_WaterStep;
    std::shared_ptr <engine::app3D::SoundDef> Sound_Constructed;
    std::shared_ptr <engine::app3D::SoundDef> Sound_StartSwimming;
    std::shared_ptr <engine::app3D::SoundDef> Sound_LeaveWater;
    std::shared_ptr <engine::app3D::SoundDef> Sound_FallIntoWater;
    std::shared_ptr <engine::app3D::SoundDef> Sound_Swim1;
    std::shared_ptr <engine::app3D::SoundDef> Sound_Swim2;
    std::shared_ptr <engine::app3D::SoundDef> Sound_TargetAcquired;
    std::shared_ptr <engine::app3D::SoundDef> Sound_TurretHeadMove;

    // GUI sounds
    std::shared_ptr <engine::app3D::SoundDef> Sound_StartAiming;
    std::shared_ptr <engine::app3D::SoundDef> Sound_StopAiming;
    std::shared_ptr <engine::app3D::SoundDef> Sound_Inventory1;
    std::shared_ptr <engine::app3D::SoundDef> Sound_Inventory2;
    std::shared_ptr <engine::app3D::SoundDef> Sound_EquipWeapon;
    std::shared_ptr <engine::app3D::SoundDef> Sound_Select1;
    std::shared_ptr <engine::app3D::SoundDef> Sound_Select2;
    std::shared_ptr <engine::app3D::SoundDef> Sound_Select3;
    std::shared_ptr <engine::app3D::SoundDef> Sound_Switch1;
    std::shared_ptr <engine::app3D::SoundDef> Sound_Negative1;
    std::shared_ptr <engine::app3D::SoundDef> Sound_GoUnderWater;
    std::shared_ptr <engine::app3D::SoundDef> Sound_LeaveFromUnderWater;
    std::shared_ptr <engine::app3D::SoundDef> Sound_HitTarget;
    std::shared_ptr <engine::app3D::SoundDef> Sound_ITakeDamage;
    std::shared_ptr <engine::app3D::SoundDef> Sound_MyDeath;
    std::shared_ptr <engine::app3D::SoundDef> Sound_UnlockUpgrade;
    std::shared_ptr <engine::app3D::SoundDef> Sound_LevelUp;
    std::shared_ptr <engine::app3D::SoundDef> Sound_SearchItemContainer;
    std::shared_ptr <engine::app3D::SoundDef> Sound_Crafted;

    // lights
    std::shared_ptr <engine::app3D::LightDef> Light_Directional;

    // factions
    std::shared_ptr <FactionDef> Faction_Neutral;
    std::shared_ptr <FactionDef> Faction_Players;

    // effects
    std::shared_ptr <EffectDef> Effect_Poof;
    std::shared_ptr <EffectDef> Effect_SmallExplosion;
    std::shared_ptr <EffectDef> Effect_Explosion;
    std::shared_ptr <EffectDef> Effect_BigExplosion;
    std::shared_ptr <EffectDef> Effect_OnHitGeneric;

    // world parts
    std::shared_ptr <WorldPartDef> WorldPart_Water;

    // faction relations
    std::vector <std::shared_ptr <FactionRelationDef>> AllFactionRelations;

    // upgrades
    std::vector <std::shared_ptr <UpgradeDef>> AllUpgrades;

    // crafting recipes
    std::vector <std::shared_ptr <CraftingRecipeDef>> AllCraftingRecipes;

private:
    void init();
};

} // namespace app

#endif // APP_DEFS_CACHE_HPP
