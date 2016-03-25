#ifndef APP_CRAFTING_WINDOW_HPP
#define APP_CRAFTING_WINDOW_HPP

#include "engine/util/Vec2.hpp"

#include <vector>

namespace engine { namespace GUI { class Window; class Button; class RectWidget; class Image; class Label; } }

namespace app
{

class Character;
class Structure;

class CraftingWindow
{
public:
    CraftingWindow(const std::shared_ptr <Character> &character);
    CraftingWindow(const std::shared_ptr <Character> &character, const std::shared_ptr <Structure> &workbench);

    void update();

    bool isClosed() const;

    bool hasWorkbench() const;
    Character &getCharacter() const;
    Structure &getWorkbench() const;

    static const float k_maxCharacterDistanceToWorkbench;

private:
    struct Row
    {
        std::shared_ptr <engine::GUI::RectWidget> rect;
        std::shared_ptr <engine::GUI::Button> craftButton;
        std::shared_ptr <engine::GUI::Image> itemImage;
        std::shared_ptr <engine::GUI::Label> itemLabel;
        std::shared_ptr <engine::GUI::Label> priceLabel;
        std::shared_ptr <engine::GUI::Label> skillsRequirementLabel;
    };

    void init();
    void recreateRows();

    static const engine::IntVec2 k_size;
    static const std::string k_title;
    static const int k_rowsCountPerPage;

    std::shared_ptr <Character> m_character;
    std::shared_ptr <Structure> m_workbench;

    std::shared_ptr <engine::GUI::Window> m_window;
    std::shared_ptr <engine::GUI::Button> m_nextButton;
    std::shared_ptr <engine::GUI::Button> m_previousButton;
    std::shared_ptr <engine::GUI::Label> m_pageLabel;
    std::vector <Row> m_rows;
    int m_rowsOffset;
};

} // namespace app

#endif // APP_CRAFTING_WINDOW_HPP
