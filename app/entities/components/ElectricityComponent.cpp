#include "ElectricityComponent.hpp"

#include "../../world/ElectricitySystem.hpp"
#include "../Structure.hpp"

namespace app
{

ElectricityComponent::ElectricityComponent(Structure &structure)
    : m_structure{structure}
{
}

bool ElectricityComponent::isWorking() const
{
    if(!isInAnyElectricitySystem())
        return false;

    return getElectricitySystem().isWorking();
}

bool ElectricityComponent::isInAnyElectricitySystem() const
{
    return !m_electricitySystem.expired();
}

ElectricitySystem &ElectricityComponent::getElectricitySystem() const
{
    const auto &shared = m_electricitySystem.lock();

    if(!shared)
        throw engine::Exception{"Electricity system is nullptr. This should have been checked before."};

    return *shared;
}

void ElectricityComponent::onRemovedFromElectricitySystem()
{
    m_electricitySystem.reset();
}

void ElectricityComponent::onAddedToElectricitySystem(const std::weak_ptr <ElectricitySystem> &electricitySystem)
{
    if(electricitySystem.expired())
        throw engine::Exception{"Electricity system is nullptr."};

    m_electricitySystem = electricitySystem;
}

} // namespace app
