#include "Core.hpp"

#include "engine/app3D/defs/ModelDef.hpp"
#include "engine/app3D/defs/TerrainDef.hpp"
#include "engine/app3D/defs/ParticleSpriteDef.hpp"
#include "engine/app3D/defs/ParticlesGroupModelDef.hpp"
#include "engine/app3D/defs/ParticlesGroupDef.hpp"
#include "engine/app3D/defs/SoundDef.hpp"
#include "engine/app3D/defs/LightDef.hpp"
#include "engine/app3D/managers/SceneManager.hpp"
#include "engine/app3D/managers/EventManager.hpp"
#include "engine/util/DefDatabase.hpp"
#include "engine/app3D/Settings.hpp"
#include "engine/app3D/Device.hpp"
#include "defs/DefsCache.hpp"
#include "defs/WorldPartDef.hpp"
#include "defs/CharacterDef.hpp"
#include "defs/ItemDef.hpp"
#include "defs/MineableDef.hpp"
#include "defs/StructureDef.hpp"
#include "defs/FactionDef.hpp"
#include "defs/FactionRelationDef.hpp"
#include "defs/CachedCollisionShapeDef.hpp"
#include "defs/AnimationFramesSetDef.hpp"
#include "defs/StructureRecipeDef.hpp"
#include "defs/UpgradeDef.hpp"
#include "defs/EffectDef.hpp"
#include "defs/CraftingRecipeDef.hpp"
#include "world/World.hpp"
#include "world/WorldPart.hpp"
#include "entities/Character.hpp"
#include "GUI/MainGUI.hpp"
#include "thisPlayer/ThisPlayer.hpp"
#include "EventReceiver.hpp"
#include "Global.hpp"
#include "SoundPool.hpp"
#include "EffectsPool.hpp"

namespace app
{

Core::Core()
    : m_appInfo{"Sandbox", engine::Version{0, 1, 0}, "Piotr Walczak"}
{
}

const engine::AppInfo &Core::getAppInfo() const
{
    return m_appInfo;
}

World &Core::getWorld()
{
    if(!m_world)
        throw engine::Exception{"World is nullptr."};

    return *m_world;
}

ThisPlayer &Core::getThisPlayer()
{
    if(!m_thisPlayer)
        throw engine::Exception{"Player is nullptr."};

    return *m_thisPlayer;
}

MainGUI &Core::getMainGUI()
{
    if(!m_mainGUI)
        throw engine::Exception{"Main GUI is nullptr."};

    return *m_mainGUI;
}

DefsCache &Core::getDefsCache()
{
    if(!m_defsCache)
        throw engine::Exception{"Defs cache is nullptr."};

    return *m_defsCache;
}

SoundPool &Core::getSoundPool()
{
    if(!m_soundPool)
        throw engine::Exception{"Sound pool is nullptr."};

    return *m_soundPool;
}

EffectsPool &Core::getEffectsPool()
{
    if(!m_effectsPool)
        throw engine::Exception{"Effects pool is nullptr."};

    return *m_effectsPool;
}

Core::~Core()
{
}

void Core::onInit(const engine::app3D::Settings &settings)
{
    TRACK;

    auto &device = getDevice();
    auto &eventManager = device.getEventManager();

    m_eventReceiver = std::make_shared <EventReceiver> ();
    eventManager.registerEventReceiver(m_eventReceiver);

    loadAllDefs(settings);

    m_soundPool = std::make_unique <SoundPool> ();
    m_effectsPool = std::make_unique <EffectsPool> ();
    m_defsCache = std::make_unique <DefsCache> ();
    m_world = std::make_unique <World> (settings);
    m_thisPlayer = std::make_unique <ThisPlayer> ();
    m_mainGUI = std::make_unique <MainGUI> ();

    device.setOnDraw3DCallback([]() {
        Global::getCore().getMainGUI().onDraw3D();
    });

    device.setOnDraw2DBeforeWidgetsCallback([]() {
        Global::getCore().getMainGUI().onDraw2DBeforeWidgets();
    });

    device.setOnDraw2DAfterWidgetsCallback([]() {
        Global::getCore().getMainGUI().onDraw2DAfterWidgets();
    });
}

bool Core::onUpdate()
{
    TRACK;

    updateCameraPos();
    getWorld().update();
    getThisPlayer().update();
    getMainGUI().update();
    getSoundPool().update();
    getEffectsPool().update();

    return true;
}

void Core::updateCameraPos()
{
    auto &sceneManager = getDevice().getSceneManager();
    auto newCameraPos = getThisPlayer().getCharacter().getEyesPosition();

    if(std::fabs(newCameraPos.y - WorldPart::k_waterHeight) < 0.2f) {
        if(newCameraPos.y > WorldPart::k_waterHeight)
            newCameraPos.y = WorldPart::k_waterHeight + 0.2f;
        else
            newCameraPos.y = WorldPart::k_waterHeight - 0.2f;
    }

    sceneManager.setCameraPosition(newCameraPos);
}

void Core::loadAllDefs(const engine::app3D::Settings &settings)
{
    for(const auto &elem : settings.mods.mods) {
        if(elem.enabled)
            loadDefs(elem.path);
    }

    getDefDatabase().callOnLoadedAllDefs();
}

void Core::loadDefs(const std::string &modPath)
{
    TRACK;

    auto &defDatabase = getDefDatabase();
    const auto &devicePtr = getDevice().getPtr();
    const auto &defsPath = "mods/" + modPath + "/defs/";

    // load engine ResourceDefs

    defDatabase.loadDefs_directory <engine::app3D::ModelDef> (defsPath + "ModelDefs", "ModelDefs");
    defDatabase.loadDefs_directory <engine::app3D::TerrainDef> (defsPath + "TerrainDefs", "TerrainDefs");
    defDatabase.loadDefs_directory <engine::app3D::ParticleSpriteDef> (defsPath + "ParticleSpriteDefs", "ParticleSpriteDefs");
    defDatabase.loadDefs_directory <engine::app3D::ParticlesGroupModelDef> (defsPath + "ParticlesGroupModelDefs", "ParticlesGroupModelDefs");
    defDatabase.loadDefs_directory <engine::app3D::ParticlesGroupDef> (defsPath + "ParticlesGroupDefs", "ParticlesGroupDefs");
    defDatabase.loadDefs_directory <engine::app3D::SoundDef> (defsPath + "SoundDefs", "SoundDefs");
    defDatabase.loadDefs_directory <engine::app3D::LightDef> (defsPath + "LightDefs", "LightDefs");

    for(auto &def : defDatabase.getDefs <engine::app3D::ResourceDef> ()) {
        def.second->setDevice(devicePtr);
    }

    // load app defs

    defDatabase.loadDefs_directory <EffectDef> (defsPath + "EffectDefs", "EffectDefs");
    defDatabase.loadDefs_directory <FactionDef> (defsPath + "FactionDefs", "FactionDefs");
    defDatabase.loadDefs_directory <FactionRelationDef> (defsPath + "FactionRelationDefs", "FactionRelationDefs");
    defDatabase.loadDefs_directory <CachedCollisionShapeDef> (defsPath + "CachedCollisionShapeDefs", "CachedCollisionShapeDefs");
    defDatabase.loadDefs_directory <AnimationFramesSetDef> (defsPath + "AnimationFramesSetDefs", "AnimationFramesSetDefs");
    defDatabase.loadDefs_directory <ItemDef> (defsPath + "ItemDefs", "ItemDefs");
    defDatabase.loadDefs_directory <MineableDef> (defsPath + "MineableDefs", "MineableDefs");
    defDatabase.loadDefs_directory <StructureDef> (defsPath + "StructureDefs", "StructureDefs");
    defDatabase.loadDefs_directory <CharacterDef> (defsPath + "CharacterDefs", "CharacterDefs");
    defDatabase.loadDefs_directory <WorldPartDef> (defsPath + "WorldPartDefs", "WorldPartDefs");
    defDatabase.loadDefs_directory <StructureRecipeDef> (defsPath + "StructureRecipeDefs", "StructureRecipeDefs");
    defDatabase.loadDefs_directory <UpgradeDef> (defsPath + "UpgradeDefs", "UpgradeDefs");
    defDatabase.loadDefs_directory <CraftingRecipeDef> (defsPath + "CraftingRecipeDefs", "CraftingRecipeDefs");
}

} // namespace app
