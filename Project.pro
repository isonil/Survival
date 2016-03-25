#-------------------------------------------------
#
# Project created by QtCreator 2014-05-09T20:42:55
#
#-------------------------------------------------

TARGET   = Project
TEMPLATE = app

QT       += core
QT       += widgets
QT       -= gui

CONFIG   += console
CONFIG   -= app_bundle

QMAKE_CXXFLAGS += -std=c++1y
QMAKE_CXXFLAGS += -Wall
QMAKE_CXXFLAGS += -Wextra

LIBSPATH = D:/Libraries/

QMAKE_CXXFLAGS += -isystem $${LIBSPATH}irrlicht-1.8.1
QMAKE_CXXFLAGS += -isystem $${LIBSPATH}Bullet/include

INCLUDEPATH += $${LIBSPATH}ENet/include
INCLUDEPATH += $${LIBSPATH}irrlicht-1.8.1
INCLUDEPATH += $${LIBSPATH}irrlicht-1.8.1/irrlicht
INCLUDEPATH += $${LIBSPATH}YAML/include
INCLUDEPATH += $${LIBSPATH}FreeType/include
INCLUDEPATH += $${LIBSPATH}Bullet/include
INCLUDEPATH += $${LIBSPATH}SFML/include
INCLUDEPATH += $${LIBSPATH}SPARK/include

LIBS += $${LIBSPATH}ENet/ENet.dll
LIBS += $${LIBSPATH}irrlicht-1.8.1/Irrlicht.dll
LIBS += $${LIBSPATH}YAML/libyaml-cpp.a
LIBS += $${LIBSPATH}FreeType/libfreetype253.a
LIBS += $${LIBSPATH}Bullet/libBulletDynamics.a
LIBS += $${LIBSPATH}Bullet/libBulletCollision.a
LIBS += $${LIBSPATH}Bullet/libBulletSoftBody.a
LIBS += $${LIBSPATH}Bullet/libLinearMath.a
LIBS += $${LIBSPATH}Bullet/libBulletFileLoader.a
LIBS += $${LIBSPATH}Bullet/libBulletWorldImporter.a
LIBS += $${LIBSPATH}Bullet/libConvexDecomposition.a
LIBS += $${LIBSPATH}Bullet/libGIMPACTUtils.a
LIBS += $${LIBSPATH}Bullet/libHACD.a
LIBS += $${LIBSPATH}Bullet/libOpenGLSupport.a
LIBS += $${LIBSPATH}SFML/sfml-system-2.dll
LIBS += $${LIBSPATH}SFML/sfml-audio-2.dll
LIBS += $${LIBSPATH}SPARK/SPARK.dll

SOURCES += main.cpp \
    engine/util/Random.cpp \
    engine/util/LogManager.cpp \
    engine/util/Version.cpp \
    engine/util/Packet.cpp \
    engine/util/Network.cpp \
    engine/util/Server.cpp \
    engine/util/Client.cpp \
    engine/app3D/Device.cpp \
    engine/util/RichText.cpp \
    engine/App3D.cpp \
    engine/util/DataFile.cpp \
    engine/util/Trace.cpp \
    engine/util/Exception.cpp \
    engine/util/Time.cpp \
    engine/util/Color.cpp \
    engine/util/AppTime.cpp \
    engine/util/QuadTree.cpp \
    engine/util/RectPacker.cpp \
    engine/GUI/GUIManager.cpp \
    engine/app3D/detail/GUIRenderer.cpp \
    engine/app3D/ext/CGUITTFont.cpp \
    engine/GUI/Event.cpp \
    engine/app3D/detail/GUITexture.cpp \
    engine/util/VideoMode.cpp \
    engine/app3D/Settings.cpp \
    engine/app3D/irrNodes/MeshBatch.cpp \
    engine/app3D/irrNodes/BillboardBatch.cpp \
    engine/app3D/managers/ShadersManager.cpp \
    engine/app3D/managers/SceneManager.cpp \
    engine/app3D/managers/ResourcesManager.cpp \
    engine/app3D/managers/MeshBatchManager.cpp \
    engine/app3D/managers/EventManager.cpp \
    engine/app3D/managers/BillboardBatchManager.cpp \
    engine/app3D/sceneNodes/SceneNode.cpp \
    engine/app3D/sceneNodes/Model.cpp \
    engine/app3D/sceneNodes/Light.cpp \
    engine/app3D/managers/PhysicsManager.cpp \
    engine/app3D/physics/RigidBody.cpp \
    engine/app3D/physics/SphereShape.cpp \
    engine/app3D/physics/HeightMapShape.cpp \
    engine/app3D/physics/BoxShape.cpp \
    engine/app3D/physics/CylinderShape.cpp \
    engine/app3D/physics/CapsuleShape.cpp \
    engine/app3D/physics/ConeShape.cpp \
    engine/app3D/physics/ConvexHullShape.cpp \
    engine/app3D/physics/StaticPlaneShape.cpp \
    app/EventReceiver.cpp \
    engine/AppInfo.cpp \
    app/Core.cpp \
    engine/EngineStaticInfo.cpp \
    app/Global.cpp \
    app/world/World.cpp \
    engine/util/Def.cpp \
    engine/util/DefDatabase.cpp \
    engine/app3D/defs/ResourceDef.cpp \
    engine/app3D/defs/ModelDef.cpp \
    engine/app3D/defs/TerrainDef.cpp \
    engine/app3D/sceneNodes/Terrain.cpp \
    app/defs/WorldPartDef.cpp \
    app/world/WorldPart.cpp \
    engine/app3D/sceneNodes/Water.cpp \
    app/defs/CharacterDef.cpp \
    engine/util/StringUtility.cpp \
    engine/app3D/IrrlichtConversions.cpp \
    app/entities/Character.cpp \
    app/entities/Entity.cpp \
    app/entities/Mineable.cpp \
    app/defs/MineableDef.cpp \
    app/entities/Item.cpp \
    app/entities/Structure.cpp \
    app/defs/ItemDef.cpp \
    app/defs/StructureDef.cpp \
    engine/app3D/physics/KinematicCharacterController.cpp \
    engine/app3D/physics/DynamicCharacterController.cpp \
    app/defs/CachedCollisionShapeDef.cpp \
    engine/app3D/physics/BvhTriangleMeshShape.cpp \
    app/world/PlacementPredicates.cpp \
    app/defs/AnimationFramesSetDef.cpp \
    app/util/InterpolatedFloat.cpp \
    app/util/InterpolatedLoopedFloatVec3.cpp \
    app/util/InterpolatedFloatVec3.cpp \
    app/defs/DefsCache.cpp \
    app/defs/UpgradeDef.cpp \
    engine/GUI/widgets/Window.cpp \
    engine/GUI/widgets/WidgetContainer.cpp \
    engine/GUI/widgets/Widget.cpp \
    engine/GUI/widgets/RectWidget.cpp \
    engine/GUI/widgets/Label.cpp \
    engine/GUI/widgets/Image.cpp \
    engine/GUI/widgets/EditBox.cpp \
    engine/GUI/widgets/Button.cpp \
    app/GUI/MainGUI.cpp \
    app/GUI/widgetPacks/InventoryWindow.cpp \
    app/GUI/immediate/HandyWindow_Inventory.cpp \
    app/GUI/immediate/HandyWindow_Construct.cpp \
    app/GUI/immediate/HandyWindow.cpp \
    engine/app3D/managers/CursorManager.cpp \
    app/itemContainers/MultiSlotItemContainer.cpp \
    app/itemContainers/SingleSlotItemContainer.cpp \
    app/GUI/widgets/MultiSlotItemContainerWidget.cpp \
    app/GUI/widgets/SingleSlotItemContainerWidget.cpp \
    app/defs/StructureRecipeDef.cpp \
    app/GUI/widgets/UpgradeTreeWidget.cpp \
    engine/GUI/IGUITexture.cpp \
    engine/app3D/physics/CollisionDetector.cpp \
    engine/app3D/physics/GhostObject.cpp \
    app/Snapper.cpp \
    app/util/Timer.cpp \
    engine/GUI/widgets/ProgressBar.cpp \
    app/defs/parts/ItemFPPProperties.cpp \
    app/defs/parts/ItemGathersResourcesProperties.cpp \
    app/defs/parts/OnUsedItem.cpp \
    app/defs/parts/WorldPartRandomMineable.cpp \
    app/entities/components/CharacterComponent.cpp \
    app/entities/components/NPCComponent.cpp \
    app/entities/components/PlayerComponent.cpp \
    app/entities/components/player/UnlockedStructures.cpp \
    app/thisPlayer/ThisPlayer.cpp \
    app/thisPlayer/FPPItemModelController.cpp \
    app/thisPlayer/ItemCurrentlyDragged.cpp \
    app/thisPlayer/StructureCurrentlyDesignated.cpp \
    app/thisPlayer/ItemInHands.cpp \
    app/thisPlayer/PointedEntity.cpp \
    app/thisPlayer/DeconstructionTracker.cpp \
    engine/util/Music.cpp \
    app/SoundPool.cpp \
    app/world/WorldPartTopographyInfo.cpp \
    app/thisPlayer/SwimTracker.cpp \
    engine/app3D/physics/CollisionFilter.cpp \
    app/world/WorldPartFreePosFinder.cpp \
    app/defs/FactionDef.cpp \
    app/defs/FactionRelationDef.cpp \
    engine/app3D/physics/Ragdoll.cpp \
    app/entities/character/CharacterStatsChange.cpp \
    app/entities/character/CharacterStatsAccumulator.cpp \
    app/entities/character/Inventory.cpp \
    app/GUI/immediate/BloodSplat.cpp \
    app/world/DateTimeManager.cpp \
    app/world/SpawnManager.cpp \
    app/defs/parts/WorldPartMob.cpp \
    app/entities/components/player/Skills.cpp \
    app/GUI/immediate/HandyWindow_SkillsAndUpgrades.cpp \
    app/GUI/immediate/ExperienceBar.cpp \
    app/entities/components/player/UnlockedUpgrades.cpp \
    app/GUI/widgetPacks/SkillsAndUpgradesWindow.cpp \
    app/GUI/immediate/Tooltip.cpp \
    app/GUI/immediate/LevelUpAnimation.cpp \
    app/defs/parts/TurretInfo.cpp \
    app/entities/components/TurretComponent.cpp \
    engine/app3D/managers/ParticlesManager.cpp \
    engine/app3D/defs/ParticleSpriteDef.cpp \
    engine/app3D/defs/ParticlesGroupModelDef.cpp \
    engine/app3D/defs/ParticlesGroupDef.cpp \
    engine/app3D/particles/ParticlesGroup.cpp \
    app/defs/EffectDef.cpp \
    app/world/Effect.cpp \
    app/EffectsPool.cpp \
    engine/app3D/irrNodes/GrassPatch.cpp \
    engine/util/WindGenerator.cpp \
    app/entities/character/CharacterStatsOrSkillsRelatedFormulas.cpp \
    engine/ext/PerlinNoise.cpp \
    app/entities/character/ProjectilesSpreadAngleManager.cpp \
    engine/util/Util.cpp \
    app/entities/components/MovingOnGroundNPCComponent.cpp \
    app/entities/components/FlyingNPCComponent.cpp \
    app/entities/components/player/SkillsRequirement.cpp \
    app/thisPlayer/RevivingTracker.cpp \
    app/world/ElectricitySystem.cpp \
    app/entities/components/ElectricityComponent.cpp \
    engine/app3D/Sound.cpp \
    engine/app3D/defs/SoundDef.cpp \
    app/thisPlayer/ItemContainerSearchTracker.cpp \
    app/GUI/widgetPacks/ItemContainerWindow.cpp \
    app/defs/CraftingRecipeDef.cpp \
    app/GUI/widgetPacks/CraftingWindow.cpp \
    app/Price.cpp \
    engine/app3D/defs/LightDef.cpp \
    app/defs/EntityDef.cpp \
    app/defs/parts/ItemsList.cpp \
    app/defs/parts/ItemsListWithUnboundedStack.cpp \
    engine/app3D/physics/ConeTwistConstraint.cpp \
    engine/app3D/physics/Armature.cpp \
    engine/app3D/IslandGenerator.cpp \
    engine/app3D/irrNodes/VerticesAndIndicesNode.cpp \
    engine/app3D/sceneNodes/Island.cpp

HEADERS += \
    engine/util/Random.hpp \
    engine/util/LogManager.hpp \
    engine/util/Version.hpp \
    engine/util/Network.hpp \
    engine/util/Packet.hpp \
    engine/util/Server.hpp \
    engine/util/Client.hpp \
    engine/app3D/Device.hpp \
    engine/util/RichText.hpp \
    engine/App3D.hpp \
    engine/util/DataFile.hpp \
    engine/util/Trace.hpp \
    engine/util/Exception.hpp \
    engine/util/Range.hpp \
    engine/util/Rect.hpp \
    engine/util/Math.hpp \
    engine/util/Vec2.hpp \
    engine/util/Vec3.hpp \
    engine/util/Time.hpp \
    engine/util/Color.hpp \
    engine/util/AppTime.hpp \
    engine/util/QuadTree.hpp \
    engine/util/Enum.hpp \
    engine/app3D/ext/irrUString.hpp \
    engine/app3D/ext/CGUITTFont.h \
    engine/util/RectPacker.hpp \
    engine/GUI/IGUIRenderer.hpp \
    engine/GUI/GUIManager.hpp \
    engine/app3D/detail/GUIRenderer.hpp \
    engine/GUI/Event.hpp \
    engine/GUI/IEventReceiver.hpp \
    engine/GUI/IGUITexture.hpp \
    engine/app3D/detail/GUITexture.hpp \
    engine/app3D/IIrrlichtObjectsHolder.hpp \
    engine/app3D/SceneManager.hpp \
    engine/util/VideoMode.hpp \
    engine/app3D/Settings.hpp \
    engine/app3D/irrNodes/MeshBatch.hpp \
    engine/app3D/irrNodes/BillboardBatch.hpp \
    engine/app3D/managers/ShadersManager.hpp \
    engine/app3D/managers/SceneManager.hpp \
    engine/app3D/managers/ResourcesManager.hpp \
    engine/app3D/managers/MeshBatchManager.hpp \
    engine/app3D/managers/EventManager.hpp \
    engine/app3D/managers/BillboardBatchManager.hpp \
    engine/app3D/sceneNodes/SceneNode.hpp \
    engine/app3D/sceneNodes/Model.hpp \
    engine/app3D/sceneNodes/Light.hpp \
    engine/app3D/managers/PhysicsManager.hpp \
    engine/app3D/physics/RigidBody.hpp \
    engine/app3D/physics/SphereShape.hpp \
    engine/app3D/physics/HeightMapShape.hpp \
    engine/app3D/physics/BoxShape.hpp \
    engine/app3D/physics/CylinderShape.hpp \
    engine/app3D/physics/CapsuleShape.hpp \
    engine/app3D/physics/ConeShape.hpp \
    engine/app3D/physics/ConvexHullShape.hpp \
    engine/app3D/physics/StaticPlaneShape.hpp \
    app/EventReceiver.hpp \
    engine/app3D/physics/CollisionShape.hpp \
    app/Core.hpp \
    engine/AppInfo.hpp \
    engine/EngineStaticInfo.hpp \
    app/Global.hpp \
    app/world/World.hpp \
    engine/util/DefDatabase.hpp \
    engine/util/Def.hpp \
    engine/ext/optional.hpp \
    engine/app3D/defs/ResourceDef.hpp \
    engine/app3D/defs/ModelDef.hpp \
    engine/app3D/defs/TerrainDef.hpp \
    engine/app3D/sceneNodes/Terrain.hpp \
    app/defs/WorldPartDef.hpp \
    app/world/WorldPart.hpp \
    engine/app3D/sceneNodes/Water.hpp \
    app/defs/CharacterDef.hpp \
    engine/util/StringUtility.hpp \
    engine/app3D/IrrlichtConversions.hpp \
    app/entities/Character.hpp \
    app/entities/Entity.hpp \
    app/entities/Mineable.hpp \
    app/defs/MineableDef.hpp \
    app/entities/Item.hpp \
    app/entities/Structure.hpp \
    app/defs/ItemDef.hpp \
    app/defs/StructureDef.hpp \
    engine/app3D/physics/KinematicCharacterController.hpp \
    engine/app3D/physics/DynamicCharacterController.hpp \
    app/defs/CachedCollisionShapeDef.hpp \
    engine/app3D/physics/BvhTriangleMeshShape.hpp \
    engine/util/DHondtDistribution.hpp \
    app/world/PlacementPredicates.hpp \
    app/defs/AnimationFramesSetDef.hpp \
    app/util/InterpolatedFloat.hpp \
    app/util/InterpolatedLoopedFloatVec3.hpp \
    app/util/InterpolatedFloatVec3.hpp \
    app/util/InterpolationType.hpp \
    app/defs/DefsCache.hpp \
    app/defs/UpgradeDef.hpp \
    engine/GUI/widgets/Window.hpp \
    engine/GUI/widgets/WidgetContainer.hpp \
    engine/GUI/widgets/Widget.hpp \
    engine/GUI/widgets/RectWidget.hpp \
    engine/GUI/widgets/Label.hpp \
    engine/GUI/widgets/Image.hpp \
    engine/GUI/widgets/EditBox.hpp \
    engine/GUI/widgets/Button.hpp \
    app/GUI/MainGUI.hpp \
    app/GUI/widgetPacks/InventoryWindow.hpp \
    app/GUI/immediate/HandyWindow_Inventory.hpp \
    app/GUI/immediate/HandyWindow_Construct.hpp \
    app/GUI/immediate/HandyWindow.hpp \
    engine/app3D/managers/CursorManager.hpp \
    app/itemContainers/SingleSlotItemContainer.hpp \
    app/itemContainers/MultiSlotItemContainer.hpp \
    app/itemContainers/IItemContainer.hpp \
    app/GUI/widgets/MultiSlotItemContainerWidget.hpp \
    app/GUI/widgets/SingleSlotItemContainerWidget.hpp \
    app/defs/StructureRecipeDef.hpp \
    app/GUI/widgets/UpgradeTreeWidget.hpp \
    engine/app3D/physics/CollisionDetector.hpp \
    engine/app3D/physics/CollisionFilter.hpp \
    engine/app3D/physics/GhostObject.hpp \
    app/Snapper.hpp \
    app/util/Timer.hpp \
    engine/GUI/widgets/ProgressBar.hpp \
    app/defs/parts/ItemFPPProperties.hpp \
    app/defs/parts/OnUsedItem.hpp \
    app/defs/parts/ItemGathersResourcesProperties.hpp \
    app/defs/parts/WorldPartRandomMineable.hpp \
    app/entities/components/CharacterComponent.hpp \
    app/entities/components/PlayerComponent.hpp \
    app/entities/components/NPCComponent.hpp \
    app/thisPlayer/ThisPlayer.hpp \
    app/entities/components/player/UnlockedStructures.hpp \
    app/thisPlayer/FPPItemModelController.hpp \
    app/thisPlayer/ItemCurrentlyDragged.hpp \
    app/thisPlayer/StructureCurrentlyDesignated.hpp \
    app/thisPlayer/ItemInHands.hpp \
    app/thisPlayer/PointedEntity.hpp \
    app/thisPlayer/DeconstructionTracker.hpp \
    engine/util/Music.hpp \
    app/SoundPool.hpp \
    app/world/GroundType.hpp \
    app/world/WorldPartTopographyInfo.hpp \
    app/thisPlayer/SwimTracker.hpp \
    app/world/WorldPartFreePosFinder.hpp \
    app/defs/FactionDef.hpp \
    app/defs/FactionRelationDef.hpp \
    engine/app3D/physics/Ragdoll.hpp \
    app/entities/character/CharacterStatsChange.hpp \
    app/entities/character/CharacterStatsAccumulator.hpp \
    app/entities/character/Inventory.hpp \
    app/GUI/immediate/BloodSplat.hpp \
    app/world/DateTimeManager.hpp \
    app/world/SpawnManager.hpp \
    app/defs/parts/WorldPartMob.hpp \
    app/entities/components/player/Skills.hpp \
    app/GUI/immediate/HandyWindow_SkillsAndUpgrades.hpp \
    app/GUI/immediate/ExperienceBar.hpp \
    app/entities/components/player/UnlockedUpgrades.hpp \
    app/GUI/widgetPacks/SkillsAndUpgradesWindow.hpp \
    app/GUI/immediate/Tooltip.hpp \
    app/GUI/immediate/LevelUpAnimation.hpp \
    app/defs/parts/TurretInfo.hpp \
    app/entities/components/TurretComponent.hpp \
    engine/app3D/managers/ParticlesManager.hpp \
    engine/app3D/defs/ParticleSpriteDef.hpp \
    engine/app3D/defs/ParticlesGroupModelDef.hpp \
    engine/app3D/defs/ParticlesGroupDef.hpp \
    engine/app3D/particles/ParticlesGroup.hpp \
    app/defs/EffectDef.hpp \
    app/world/Effect.hpp \
    app/EffectsPool.hpp \
    engine/app3D/irrNodes/GrassPatch.hpp \
    engine/util/WindGenerator.hpp \
    app/entities/character/CharacterStatsOrSkillsRelatedFormulas.hpp \
    engine/ext/PerlinNoise.hpp \
    app/entities/character/ProjectilesSpreadAngleManager.hpp \
    engine/util/Util.hpp \
    app/entities/components/MovingOnGroundNPCComponent.hpp \
    app/entities/components/FlyingNPCComponent.hpp \
    app/entities/components/player/SkillsRequirement.hpp \
    app/thisPlayer/RevivingTracker.hpp \
    app/world/ElectricitySystem.hpp \
    app/entities/components/ElectricityComponent.hpp \
    engine/app3D/Sound.hpp \
    engine/app3D/defs/SoundDef.hpp \
    app/thisPlayer/ItemContainerSearchTracker.hpp \
    app/GUI/widgetPacks/ItemContainerWindow.hpp \
    app/defs/CraftingRecipeDef.hpp \
    app/GUI/widgetPacks/CraftingWindow.hpp \
    app/Price.hpp \
    engine/app3D/defs/LightDef.hpp \
    app/defs/EntityDef.hpp \
    app/defs/parts/ItemsList.hpp \
    app/defs/parts/ItemsListWithUnboundedStack.hpp \
    engine/app3D/physics/ConeTwistConstraint.hpp \
    engine/app3D/physics/Armature.hpp \
    engine/app3D/IslandGenerator.hpp \
    engine/app3D/irrNodes/VerticesAndIndicesNode.hpp \
    engine/app3D/sceneNodes/Island.hpp

OTHER_FILES += \
    engine/app3D/ext/CGUITTFont.cpp.txt
