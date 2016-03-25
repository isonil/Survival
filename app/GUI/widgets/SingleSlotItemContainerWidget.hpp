#ifndef APP_SINGLE_SLOT_ITEM_CONTAINER_WIDGET_HPP
#define APP_SINGLE_SLOT_ITEM_CONTAINER_WIDGET_HPP

#include "engine/GUI/widgets/Widget.hpp"

namespace engine { namespace GUI { class IGUITexture; } }

namespace app
{

class SingleSlotItemContainer;

class SingleSlotItemContainerWidget : public engine::GUI::Widget, public engine::Tracked <SingleSlotItemContainerWidget>
{
public:
    SingleSlotItemContainerWidget(engine::GUI::WidgetContainer *parent, const std::shared_ptr <engine::GUI::IGUIRenderer> &renderer, const engine::IntRect &rect, const std::shared_ptr <SingleSlotItemContainer> &itemContainer, const std::string &text);

    void onEvent(engine::GUI::Event &event) override;
    void update(const engine::AppTime &time, bool nonOccludedMouseOver) override;
    void draw() const override;
    bool anyNonInternalChildWidgets() const override;

    static const int k_slotSize;

private:
    using base = Widget;

    static const std::string k_slotTexturePath;

    std::shared_ptr <SingleSlotItemContainer> m_itemContainer;
    std::string m_text;
    std::shared_ptr <engine::GUI::IGUITexture> m_slotTexture;
    bool m_shouldHighlight;
};

} // namespace app

#endif // APP_SINGLE_SLOT_ITEM_CONTAINER_WIDGET_HPP
