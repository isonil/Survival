#ifndef APP_PRICE_HPP
#define APP_PRICE_HPP

#include "engine/util/DataFile.hpp"
#include "engine/util/RichText.hpp"

namespace app
{

class ItemDef;
class Inventory;

class Price : public engine::DataFile::Saveable
{
public:
    class RequiredItem : public engine::DataFile::Saveable
    {
    public:
        RequiredItem();

        void expose(engine::DataFile::Node &node) override;

        ItemDef &getItemDef() const;
        int getCount() const;

    private:
        std::string m_itemDef_defName;
        std::shared_ptr <ItemDef> m_itemDef;
        int m_count;
    };

    void expose(engine::DataFile::Node &node) override;

    const std::vector <RequiredItem> &getRequiredItems() const;
    int getAllRequiredItemsTotalCount() const;

    std::string toString() const;
    engine::RichText toRichText(const Inventory &inventory, char delim) const;

private:
    std::vector <RequiredItem> m_requiredItems;
};

} // namespace app

#endif // APP_PRICE_HPP
