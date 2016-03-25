#ifndef APP_ITEM_IN_HANDS_HPP
#define APP_ITEM_IN_HANDS_HPP

#include "../util/Timer.hpp"

namespace app
{

class Item;

class ItemInHands
{
public:
    ItemInHands();

    void update();

    bool hasItem() const;
    Item &getItem() const;

    void tryUse();
    void tryStartContinuousUse();
    void tryStopContinuousUse();

private:
    bool canBeUsedContinuously() const;

    bool m_continuousUse;
    std::vector <Timer> m_onUsedTimers;
};

} // namespace app

#endif // APP_ITEM_IN_HANDS_HPP
