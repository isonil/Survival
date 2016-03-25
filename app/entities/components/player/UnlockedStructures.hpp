#ifndef APP_UNLOCKED_STRUCTURES_HPP
#define APP_UNLOCKED_STRUCTURES_HPP

#include <vector>
#include <memory>

namespace app
{

class StructureRecipeDef;

class UnlockedStructures
{
public:
    UnlockedStructures();

    void unlock(const std::shared_ptr <StructureRecipeDef> &def);
    const std::vector <std::shared_ptr <StructureRecipeDef>> &getUnlocked() const;

private:
    void unlockUnlockedByDefault();

    std::vector <std::shared_ptr <StructureRecipeDef>> m_unlocked;
};

} // namespace app

#endif // APP_UNLOCKED_STRUCTURES_HPP
