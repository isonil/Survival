#ifndef APP_INVENTORY_WINDOW_HPP
#define APP_INVENTORY_WINDOW_HPP

#include "engine/util/Vec2.hpp"

namespace engine { namespace GUI { class Window; class Image; } }

namespace app
{

class Inventory;
class MultiSlotItemContainerWidget;
class SingleSlotItemContainerWidget;

class InventoryWindow
{
public:
    InventoryWindow(Inventory &inventory);

    void update();

    const std::shared_ptr <engine::GUI::Window> &getWindow() const;

private:
    static const engine::IntVec2 k_size;
    static const std::string k_title;
    static const std::string k_silhouetteTexturePath;

    std::shared_ptr <engine::GUI::Window> m_window;
    std::shared_ptr <engine::GUI::Image> m_silhouette;
    std::shared_ptr <MultiSlotItemContainerWidget> m_multiSlotItemContainerWidget;
    std::shared_ptr <SingleSlotItemContainerWidget> m_handsItemContainerWidget;
    std::shared_ptr <SingleSlotItemContainerWidget> m_armorItemContainerWidget;
    std::shared_ptr <SingleSlotItemContainerWidget> m_specialItemContainerWidget;
};

} // namespace app

#endif // APP_INVENTORY_WINDOW_HPP
