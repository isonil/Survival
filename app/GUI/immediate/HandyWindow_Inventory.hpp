#ifndef APP_HANDY_WINDOW_INVENTORY_HPP
#define APP_HANDY_WINDOW_INVENTORY_HPP

#include "HandyWindow.hpp"

namespace app
{

class Item;

class HandyWindow_Inventory : public HandyWindow
{
public:
    HandyWindow_Inventory();

    void update() override;
    void onEvent(engine::GUI::Event &event) override;
    void draw() override;
    Type getType() const override;

private:
    using base = HandyWindow;

    void tryChangeSelection(bool goUp);

    void updateCurrentEquippableItems();
    void updateEquippableItemsOffset();

    static const engine::IntVec2 k_size;
    static const int k_visibleEquippableItemsCount;
    static const std::string k_upArrowTexturePath;
    static const std::string k_downArrowTexturePath;

    std::vector <std::shared_ptr <Item>> m_currentEquippableItems;
    int m_equippableItemsOffset;
    std::shared_ptr <engine::GUI::IGUITexture> m_upArrowTexture;
    std::shared_ptr <engine::GUI::IGUITexture> m_downArrowTexture;
};

} // namespace app

#endif // APP_HANDY_WINDOW_INVENTORY_HPP
