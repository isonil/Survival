#include "ElectricitySystem.hpp"

#include "../entities/components/ElectricityComponent.hpp"
#include "engine/app3D/managers/SceneManager.hpp"
#include "engine/app3D/sceneNodes/Model.hpp"
#include "../entities/Structure.hpp"
#include "../defs/DefsCache.hpp"
#include "../defs/StructureDef.hpp"
#include "engine/util/Color.hpp"
#include "engine/app3D/Device.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "World.hpp"

#include <algorithm>

namespace app
{

std::shared_ptr <ElectricitySystem> ElectricitySystem::create()
{
    // ElectricitySystem constructor is private
    std::shared_ptr <ElectricitySystem> ptr(new ElectricitySystem{});

    ptr->m_ptr = ptr;

    return ptr;
}

void ElectricitySystem::update()
{
    auto &core = Global::getCore();
    auto &defsCache = core.getDefsCache();
    auto &sceneManager = core.getDevice().getSceneManager();

    for(auto &mapElem : m_members) {
        auto &elem = mapElem.second;

        E_DASSERT(elem.structure, "Structure is nullptr.");

        bool isGenerator{elem.structure->getDef().getGeneratedPower() > 0};

        if(m_overlay_forUpdate == Overlay::GraySymbols || m_overlay_forUpdate == Overlay::GraySymbolsAndLines) {
            if(!elem.symbol ||
               (&elem.symbol->getDef() != defsCache.Model_ElectricitySymbol_Normal_Gray.get() &&
                &elem.symbol->getDef() != defsCache.Model_ElectricitySymbol_Generator_Gray.get())) {
                if(isGenerator)
                    elem.symbol = sceneManager.addModel(defsCache.Model_ElectricitySymbol_Generator_Gray);
                else
                    elem.symbol = sceneManager.addModel(defsCache.Model_ElectricitySymbol_Normal_Gray);
            }
        }
        else if(m_overlay_forUpdate == Overlay::NormalSymbolsAndLines) {
            if(!elem.symbol ||
               (&elem.symbol->getDef() != defsCache.Model_ElectricitySymbol_Normal_Normal.get() &&
                &elem.symbol->getDef() != defsCache.Model_ElectricitySymbol_Generator_Normal.get())) {
                if(isGenerator)
                    elem.symbol = sceneManager.addModel(defsCache.Model_ElectricitySymbol_Generator_Normal);
                else
                    elem.symbol = sceneManager.addModel(defsCache.Model_ElectricitySymbol_Normal_Normal);
            }
        }
        else if(m_overlay_forUpdate == Overlay::RedSymbolsAndLines) {
            if(!elem.symbol ||
               (&elem.symbol->getDef() != defsCache.Model_ElectricitySymbol_Normal_Red.get() &&
                &elem.symbol->getDef() != defsCache.Model_ElectricitySymbol_Generator_Red.get())) {
                if(isGenerator)
                    elem.symbol = sceneManager.addModel(defsCache.Model_ElectricitySymbol_Generator_Red);
                else
                    elem.symbol = sceneManager.addModel(defsCache.Model_ElectricitySymbol_Normal_Red);
            }
        }
        else
            elem.symbol.reset();

        if(elem.symbol)
            elem.symbol->setPosition(elem.structure->getCenterPosition());
    }

    auto removedAnything = false;

    for(auto it = m_members.begin(); it != m_members.end();) {
        E_DASSERT(it->second.structure, "Structure is nullptr.");

        const auto &electricityComponent = it->second.structure->getElectricityComponent();

        if(!it->second.structure->isInWorld() ||
           !electricityComponent.isInAnyElectricitySystem() ||
           &electricityComponent.getElectricitySystem() != this) {
            it = removeMember(it);
            removedAnything = true;
        }
        else
            ++it;
    }

    // overlay must be set each frame again
    m_overlay_forUpdate = Overlay::None;

    if(removedAnything)
        divideGraphIfBrokeConnection();

    if(!isWorking())
        setOverlay(Overlay::RedSymbolsAndLines);
}

void ElectricitySystem::draw3D()
{
    engine::Color lineColor;

    if(m_overlay_forRender == Overlay::GraySymbolsAndLines)
        lineColor = {0.3f, 0.3f, 0.3f};
    else if(m_overlay_forRender == Overlay::NormalSymbolsAndLines)
        lineColor = {1.f, 1.f, 0.3f};
    else if(m_overlay_forRender == Overlay::RedSymbolsAndLines)
        lineColor = {1.f, 0.3f, 0.3f};
    else
        return;

    auto &sceneManager = Global::getCore().getDevice().getSceneManager();

    sceneManager.setLineMaterial();

    for(const auto &mapElem : m_members) {
        const auto &elem = mapElem.second;

        E_DASSERT(elem.structure, "Structure is nullptr.");

        for(int conn : elem.connections) {
            auto it = m_members.find(conn);

            E_DASSERT(it != m_members.end(), "There is a connection which points to node which is not a part of this graph. "
                                             "This means that somewhere node has been removed without removing connections.");

            E_DASSERT(it->second.structure, "Structure is nullptr.");

            sceneManager.draw3DLine(elem.structure->getCenterPosition(),
                                    it->second.structure->getCenterPosition(),
                                    lineColor);
        }
    }

    // overlay must be set each frame again
    m_overlay_forRender = Overlay::None;
}

void ElectricitySystem::setOverlay(Overlay overlay)
{
    // we use priority to determine which setOverlay call was more important,
    // so that if someone somewhere sets overlay to red, then it will be more important
    // than setting it to gray
    // (overlay is reset to None each frame anyway)

    m_overlay_forUpdate = combineOverlays(m_overlay_forUpdate, overlay);
    m_overlay_forRender = combineOverlays(m_overlay_forRender, overlay);
}

int ElectricitySystem::getRequiredPower() const
{
    return m_cachedRequiredPower;
}

int ElectricitySystem::getGeneratedPower() const
{
    return m_cachedGeneratedPower;
}

int ElectricitySystem::getUnusedPower() const
{
    return std::max(0, getGeneratedPower() - getRequiredPower());
}

bool ElectricitySystem::isWorking() const
{
    return getGeneratedPower() >= getRequiredPower();
}

bool ElectricitySystem::isStructurePresent(const Structure &structure)
{
    return m_members.find(structure.getEntityID()) != m_members.end();
}

void ElectricitySystem::removeStructure(const Structure &structure)
{
    auto it = m_members.find(structure.getEntityID());

    if(it != m_members.end()) {
        removeMember(it);
        divideGraphIfBrokeConnection();
    }
}

void ElectricitySystem::addStructure(std::shared_ptr <Structure> structure, std::weak_ptr <Structure> optionalConnection)
{
    if(!structure)
        throw engine::Exception{"Tried to add nullptr structure to electricity system."};

    if(!structure->isInWorld()) {
        E_WARNING("Tried to add structure which isn't in world to electricity system.");
        return;
    }

    if(isStructurePresent(*structure))
        return;

    Member member;
    member.structure = structure;

    const auto &optionalConnectionShared = optionalConnection.lock();

    if(optionalConnectionShared) {
        if(isStructurePresent(*optionalConnectionShared)) {
            member.connections.push_back(optionalConnectionShared->getEntityID());

            auto it = m_members.find(optionalConnectionShared->getEntityID());

            E_DASSERT(it != m_members.end(), "Could not find electricity system member with given entity ID. This should have been checked before.");

            it->second.connections.push_back(structure->getEntityID());
        }
        else {
            E_WARNING("Tried to add structure to electricity system, connected to a structure which is not in the same electricity system.");
        }
    }

    m_members.emplace(structure->getEntityID(), member);

    auto &electricityComponent = structure->getElectricityComponent();

    if(electricityComponent.isInAnyElectricitySystem())
        electricityComponent.onRemovedFromElectricitySystem();

    electricityComponent.onAddedToElectricitySystem(m_ptr);

    recache();
    divideGraphIfBrokeConnection();
}

bool ElectricitySystem::isEmpty() const
{
    return m_members.empty();
}

ElectricitySystem::~ElectricitySystem()
{
    for(const auto &elem : m_members) {
        E_DASSERT(elem.second.structure, "Structure is nullptr.");
        elem.second.structure->getElectricityComponent().onRemovedFromElectricitySystem();
    }
}

const float ElectricitySystem::k_maxConnectionLength{20.f};

ElectricitySystem::ElectricitySystem()
    : m_overlay_forUpdate{Overlay::None},
      m_overlay_forRender{Overlay::None},
      m_cachedRequiredPower{},
      m_cachedGeneratedPower{}
{
}

ElectricitySystem::Overlay ElectricitySystem::combineOverlays(ElectricitySystem::Overlay oldOverlay, ElectricitySystem::Overlay newOverlay)
{
    // setting Overlay to None should be allowed (even if Overlay::None has the lowest priority)
    if(newOverlay == Overlay::None)
        return newOverlay;

    int oldPriority{getOverlayPriority(oldOverlay)};
    int newPriority{getOverlayPriority(newOverlay)};

    return newPriority >= oldPriority ? newOverlay : oldOverlay;
}

int ElectricitySystem::getOverlayPriority(ElectricitySystem::Overlay overlay)
{
    switch(overlay) {
    case Overlay::None:
        return 0;
    case Overlay::GraySymbols:
        return 1;
    case Overlay::GraySymbolsAndLines:
        return 2;
    case Overlay::NormalSymbolsAndLines:
        return 3;
    case Overlay::RedSymbolsAndLines:
        return 4;
    default:
        return 5;
    }
}

std::unordered_map <int, ElectricitySystem::Member>::iterator ElectricitySystem::removeMember(std::unordered_map <int, ElectricitySystem::Member>::iterator it)
{
    if(it == m_members.end())
        return m_members.end();

    E_DASSERT(it->second.structure, "Structure is nullptr.");

    int entityID{it->second.structure->getEntityID()};

    // notify the structure

    it->second.structure->getElectricityComponent().onRemovedFromElectricitySystem();

    // remove it

    auto newIt = m_members.erase(it);

    // now remove all connections which used this member

    for(auto &mapElem : m_members) {
        auto &elem = mapElem.second;

        for(size_t i = 0; i < elem.connections.size();) {
            if(elem.connections[i] == entityID) {
                std::swap(elem.connections[i], elem.connections.back());
                elem.connections.pop_back();
            }
            else
                ++i;
        }
    }

    recache();

    return newIt;

    // note that connection in the graph could be broken (the graph could be split into 2 graphs),
    // so it should be checked somewhere after calling this method
}

void ElectricitySystem::recache()
{
    m_cachedRequiredPower = 0;
    m_cachedGeneratedPower = 0;

    for(const auto &elem : m_members) {
        E_DASSERT(elem.second.structure, "Structure is nullptr.");

        const auto &structureDef = elem.second.structure->getDef();

        m_cachedRequiredPower += structureDef.getRequiredPower();
        m_cachedGeneratedPower += structureDef.getGeneratedPower();
    }
}

void ElectricitySystem::divideGraphIfBrokeConnection()
{
    // for each node we'll do flood-fill to see into how many graphs this graph should break

    for(auto &elem : m_members) {
        elem.second.group_workingVar = -1;
    }

    int currentGroup{};

    for(auto &elem : m_members) {
        if(elem.second.group_workingVar >= 0)
            continue;

        divideGraphIfBrokeConnection_setGroup(elem.second, currentGroup);
        ++currentGroup;
    }

    // (group 0 is the group that stays here)

    auto &world = Global::getCore().getWorld();

    for(int i = 1; i < currentGroup; ++i) {
        std::shared_ptr <ElectricitySystem> electricitySystem;
        std::shared_ptr <Structure> firstStructureInNewElectricitySystem;

        for(const auto &elem : m_members) {
            if(elem.second.group_workingVar == i) {
                E_DASSERT(elem.second.structure, "Structure is nullptr.");

                // we notify structure that it has been removed from this electricity system,
                // (we'll remove it from the container in a moment)
                elem.second.structure->getElectricityComponent().onRemovedFromElectricitySystem();

                if(!electricitySystem) {
                    firstStructureInNewElectricitySystem = elem.second.structure;
                    electricitySystem = world.addElectricitySystem(elem.second.structure);

                    E_DASSERT(electricitySystem, "Could not add electricity system.");
                }
                else // we need to make a 'temporary connection' with first structure (we'll overwrite connections later anyway)
                    electricitySystem->addStructure(elem.second.structure, firstStructureInNewElectricitySystem);
            }
        }

        E_DASSERT(electricitySystem, "Electricity system is nullptr. It's impossible since there should be at least 1 moved node.");

        // now we can rewrite connections (they must be valid since they are in the same sub-graph)

        for(auto it = m_members.begin(); it != m_members.end();) {
            if(it->second.group_workingVar == i) {
                auto newSystemIt = electricitySystem->m_members.find(it->second.structure->getEntityID());

                E_DASSERT(newSystemIt != electricitySystem->m_members.end(), "Added structure to electricity system but it's not there.");

                newSystemIt->second.connections = it->second.connections;

                // simple erase, all notifications etc. have been already handled
                it = m_members.erase(it);
            }
            else
                ++it;
        }
    }

    if(currentGroup > 1)
        recache();
}

void ElectricitySystem::divideGraphIfBrokeConnection_setGroup(Member &member, int group)
{
    if(group < 0)
        return;

    member.group_workingVar = group;

    for(int elem : member.connections) {
        auto it = m_members.find(elem);

        E_DASSERT(it != m_members.end(), "There is a connection which points to node which is not a part of this graph. "
                                         "This means that somewhere node has been removed without removing connections.");

        if(it->second.group_workingVar < 0)
            divideGraphIfBrokeConnection_setGroup(it->second, group);
    }
}

} // namespace app
