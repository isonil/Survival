#ifndef APP_ITEM_CONTAINER_WINDOW_HPP
#define APP_ITEM_CONTAINER_WINDOW_HPP

#include <memory>

namespace engine { namespace GUI { class Window; } }

namespace app
{

class MultiSlotItemContainer;
class MultiSlotItemContainerWidget;

class ItemContainerWindow
{
public:
    ItemContainerWindow(const std::shared_ptr <MultiSlotItemContainer> &itemContainer, const std::shared_ptr <engine::GUI::Window> &optionalInventoryWindowToSnapTo = {});

    void update(const std::string &title, const std::shared_ptr <MultiSlotItemContainer> &itemContainer);
    bool isClosed() const;

private:
    std::shared_ptr <engine::GUI::Window> m_window;
    std::shared_ptr <MultiSlotItemContainerWidget> m_multiSlotItemContainerWidget;
};

} // namespace app

#endif // APP_ITEM_CONTAINER_WINDOW_HPP
