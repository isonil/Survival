#ifndef APP_ELECTRICITY_COMPONENT_HPP
#define APP_ELECTRICITY_COMPONENT_HPP

#include <memory>

namespace app
{

class Structure;
class ElectricitySystem;

class ElectricityComponent
{
public:
    ElectricityComponent(Structure &structure);

    bool isWorking() const;

    bool isInAnyElectricitySystem() const;
    ElectricitySystem &getElectricitySystem() const;

    void onRemovedFromElectricitySystem();
    void onAddedToElectricitySystem(const std::weak_ptr <ElectricitySystem> &electricitySystem);

private:
    Structure &m_structure;

    std::weak_ptr <ElectricitySystem> m_electricitySystem;
};

} // namespace app

#endif // APP_ELECTRICITY_COMPONENT_HPP
