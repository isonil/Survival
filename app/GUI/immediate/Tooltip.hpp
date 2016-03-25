#ifndef APP_TOOLTIP_HPP
#define APP_TOOLTIP_HPP

#include <string>
#include <memory>

namespace app
{

class UpgradeDef;
class ItemDef;

class Tooltip
{
public:
    Tooltip();

    void update();
    void draw();

    void setText(const std::string &str);
    void setUpgradeDef(const std::weak_ptr <UpgradeDef> &upgradeDef);
    void setItemDef(const std::weak_ptr <ItemDef> &itemDef);

private:
    enum class InfoType
    {
        None,
        Text,
        UpgradeDef,
        ItemDef
    };

    bool m_alreadyDrawn;

    InfoType m_infoType;
    std::string m_text;
    std::weak_ptr <UpgradeDef> m_upgradeDef;
    std::weak_ptr <ItemDef> m_itemDef;
};

} // namespace app

#endif // APP_TOOLTIP_HPP

