#ifndef APP_HANDY_WINDOW_CONSTRUCT_HPP
#define APP_HANDY_WINDOW_CONSTRUCT_HPP

#include "../../defs/StructureDef.hpp"
#include "engine/util/RichText.hpp"
#include "HandyWindow.hpp"

namespace app
{

class StructureRecipeDef;

class HandyWindow_Construct : public HandyWindow
{
public:
    HandyWindow_Construct();

    void update() override;
    void onEvent(engine::GUI::Event &event) override;
    void draw() override;
    Type getType() const override;

    ~HandyWindow_Construct() override;

private:
    using base = HandyWindow;

    void tryChangeSelection(bool goUp);
    void tryChangeCategory(bool goRight);

    void updateCategoriesText();
    void updateCurrentRecipes();
    void updateUnlockedStructuresOffset();

    static const engine::IntVec2 k_size;
    static const int k_visibleRecipesCount;
    static const std::string k_upArrowTexturePath;
    static const std::string k_downArrowTexturePath;

    StructureDef::Category m_structureDefCategory;
    int m_unlockedStructuresOffset;
    engine::RichText m_categoriesText;
    std::vector <std::shared_ptr <StructureRecipeDef>> m_currentRecipes;
    std::vector <StructureDef::Category> m_availableCategories;
    std::shared_ptr <engine::GUI::IGUITexture> m_upArrowTexture;
    std::shared_ptr <engine::GUI::IGUITexture> m_downArrowTexture;
};

} // namespace app

#endif // APP_HANDY_WINDOW_CONSTRUCT_HPP
