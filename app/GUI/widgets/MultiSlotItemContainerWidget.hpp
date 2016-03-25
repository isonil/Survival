#ifndef APP_MULTI_SLOT_ITEM_CONTAINER_WIDGET_HPP
#define APP_MULTI_SLOT_ITEM_CONTAINER_WIDGET_HPP

#include "../../itemContainers/MultiSlotItemContainer.hpp"
#include "engine/GUI/widgets/Widget.hpp"

namespace engine { namespace GUI { class IGUITexture; } }

namespace app
{

class Item;

class MultiSlotItemContainerWidget : public engine::GUI::Widget, public engine::Tracked <MultiSlotItemContainerWidget>
{
public:
    MultiSlotItemContainerWidget(engine::GUI::WidgetContainer *parent, const std::shared_ptr <engine::GUI::IGUIRenderer> &renderer, const engine::IntRect &rect, const std::shared_ptr <MultiSlotItemContainer> &itemContainer);

    void onEvent(engine::GUI::Event &event) override;
    void update(const engine::AppTime &time, bool nonOccludedMouseOver) override;
    void draw() const override;
    bool anyNonInternalChildWidgets() const override;

    const std::shared_ptr <MultiSlotItemContainer> &getMultiSlotItemContainerPtr() const;

    static const int k_slotSize;

private:
    using base = Widget;

    std::experimental::optional <MultiSlotItemContainer::ItemInContainer> tryGetItemAtMousePos(const engine::IntVec2 &pos) const;
    std::experimental::optional <engine::IntVec2> getExactSlotTilePosAssumingNonOccluded(const engine::IntVec2 &pos) const;
    std::experimental::optional <engine::IntVec2> getFreeSlotTile(const Item &item, const engine::IntVec2 &pos);
    std::experimental::optional <engine::IntVec2> getFreeSlotTileForItemCurrentlyDraggedAssumingNonOccluded(const engine::IntVec2 &cursorPos);

    static const std::string k_slotTexturePath;

    std::shared_ptr <MultiSlotItemContainer> m_itemContainer;
    std::shared_ptr <engine::GUI::IGUITexture> m_slotTexture;
    engine::IntRect m_highlightRect;
};

} // namespace app

#endif // APP_MULTI_SLOT_ITEM_CONTAINER_WIDGET_HPP
