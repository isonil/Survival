#include "MultiSlotItemContainer.hpp"

#include "../entities/Item.hpp"
#include "../defs/ItemDef.hpp"

namespace app
{

engine::IntRect MultiSlotItemContainer::ItemInContainer::getRect() const
{
    if(!item)
        throw engine::Exception{"Item is nullptr."};

    return {position, item->getDef().getSizeInInventory()};
}

MultiSlotItemContainer::MultiSlotItemContainer(const engine::IntVec2 &size)
    : m_size{size}
{
    if(size.x < 0 || size.y < 0)
        throw engine::Exception{"Size is negative."};
}

int MultiSlotItemContainer::tryAddItem(std::shared_ptr <Item> item)
{
    TRACK;

    if(!item)
        throw engine::Exception{"Can't insert nullptr item."};

    return tryAddItem(item, item->getStack());
}

int MultiSlotItemContainer::tryAddItem(std::shared_ptr <Item> item, int stack)
{
    TRACK;

    if(!item)
        throw engine::Exception{"Can't insert nullptr item."};

    if(stack <= 0)
        throw engine::Exception{"Can't insert <= 0 stack."};

    if(stack > item->getStack())
        throw engine::Exception{"Can't insert bigger stack than there are actual items."};

    // first, try to merge with other stacks

    int leftStack{stack};

    for(auto &elem : m_items) {
        E_DASSERT(elem.item, "Item is nullptr.");

        // can merge only if has the same def
        if(elem.item->getDefPtr() != item->getDefPtr())
            continue;

        // can't merge with itself
        if(elem.item == item)
            continue;

        int taken{elem.item->tryMergeStack(*item, leftStack)};

        leftStack -= taken;

        if(leftStack <= 0)
            break;
    }

    if(leftStack <= 0)
        return stack;

    // now search for free slot, and try to insert remaining stack

    int alreadyTaken{stack - leftStack};

    const auto &freeSpace = getFreeSpacePosition(item->getDef().getSizeInInventory());

    if(freeSpace)
        return alreadyTaken + tryAddItem(item, *freeSpace, leftStack);
    else
        return alreadyTaken;
}

bool MultiSlotItemContainer::tryRemoveItem(int itemEntityID)
{
    TRACK;

    auto it = std::find_if(m_items.begin(), m_items.end(), [itemEntityID](const ItemInContainer &itemInContainer) {
        E_DASSERT(itemInContainer.item, "Item is nullptr.");
        return itemInContainer.item->getEntityID() == itemEntityID;
    });

    if(it != m_items.end()) {
        m_items.erase(it);
        return true;
    }

    return false;
}

int MultiSlotItemContainer::tryAddItem(std::shared_ptr <Item> item, const engine::IntVec2 &pos)
{
    TRACK;

    if(!item)
        throw engine::Exception{"Can't insert nullptr item."};

    return tryAddItem(item, pos, item->getStack());
}

int MultiSlotItemContainer::tryAddItem(std::shared_ptr <Item> item, const engine::IntVec2 &pos, int stack)
{
    TRACK;

    if(!item)
        throw engine::Exception{"Can't insert nullptr item."};

    if(stack <= 0)
        throw engine::Exception{"Can't insert <= 0 stack."};

    int itemCurrentStack{item->getStack()};

    if(stack > itemCurrentStack)
        throw engine::Exception{"Can't insert bigger stack than there are actual items."};

    auto currentItemAtThisPos = std::find_if(m_items.begin(), m_items.end(), [&pos](const ItemInContainer &itemInContainer) {
        return itemInContainer.position == pos;
    });

    if(currentItemAtThisPos == m_items.end()) {
        auto sameObject = std::find_if(m_items.begin(), m_items.end(), [&item](const ItemInContainer &itemInContainer) {
            return itemInContainer.item == item;
        });

        bool rectWouldFit{wouldFit({pos, item->getDef().getSizeInInventory()})};

        if(sameObject != m_items.end()) {
            // if this item is already here, then just try to move it or split it.
            // we will return 0 here, because nothing actually gets inserted

            if(rectWouldFit) {
                if(stack == itemCurrentStack) // we want to move whole stack, we just change current object's position
                    sameObject->position = pos;
                else // we split object which is already in this container
                    m_items.push_back({item->split(stack), pos});
            }

            return 0;
        }

        // insert item at free pos
        if(rectWouldFit) {
            if(stack == itemCurrentStack) // insert whole stack
                m_items.push_back({item, pos});
            else // split stack
                m_items.push_back({item->split(stack), pos});
        }

        return stack;
    }
    else {
        E_DASSERT(currentItemAtThisPos->item, "Item is nullptr.");

        if(currentItemAtThisPos->item == item)
            return 0;

        if(currentItemAtThisPos->item->getDefPtr() == item->getDefPtr())
            return currentItemAtThisPos->item->tryMergeStack(*item, stack);
    }

    return 0;
}

std::shared_ptr <Item> MultiSlotItemContainer::tryGetItemAt(const engine::IntVec2 &pos) const
{
    auto it = std::find_if(m_items.begin(), m_items.end(), [&pos](const ItemInContainer &itemInContainer) {
        return itemInContainer.getRect().contains(pos);
    });

    if(it != m_items.end())
        return it->item;

    return nullptr;
}

bool MultiSlotItemContainer::wouldFit(const Item &item, const engine::IntVec2 &pos) const
{
    const auto &sizeInInventory = item.getDef().getSizeInInventory();
    engine::IntRect rect{pos, sizeInInventory};

    if(!engine::IntRect{{0, 0}, m_size}.fullyContains(rect))
        return false;

    if(wouldFit(rect))
        return true;

    bool any{std::any_of(m_items.begin(), m_items.end(), [&item, &pos](const ItemInContainer &itemInContainer) {
        E_DASSERT(itemInContainer.item, "Item is nullptr.");

        return itemInContainer.position == pos && // at the exact same position
               itemInContainer.item.get() != &item && // not the same item
               itemInContainer.item->getDefPtr() == item.getDefPtr() && // with the same def
               !itemInContainer.item->hasFullStack(); // without full stack
    })};

    return any;
}

bool MultiSlotItemContainer::wouldFit(const engine::IntRect &rect) const
{
    if(!engine::IntRect{{0, 0}, m_size}.fullyContains(rect))
        return false;

    bool any{std::any_of(m_items.begin(), m_items.end(), [&rect](const ItemInContainer &itemInContainer) {
        E_DASSERT(itemInContainer.item, "Item is nullptr.");

        engine::IntRect existingRect{itemInContainer.position, itemInContainer.item->getDef().getSizeInInventory()};

        return existingRect.intersects(rect);
    })};

    return !any;
}

const std::vector <MultiSlotItemContainer::ItemInContainer> &MultiSlotItemContainer::getItems() const
{
    return m_items;
}

const engine::IntVec2 &MultiSlotItemContainer::getSize() const
{
    return m_size;
}

std::experimental::optional <engine::IntVec2> MultiSlotItemContainer::getFreeSpacePosition(const engine::IntVec2 &size)
{
    for(int i = 0; i <= m_size.y - size.y; ++i) {
        for(int j = 0; j <= m_size.x - size.x; ++j) {
            engine::IntVec2 pos{j, i};

            if(wouldFit({pos, size}))
                return pos;
        }
    }

    return std::experimental::optional <engine::IntVec2> {};
}

} // namespace app
