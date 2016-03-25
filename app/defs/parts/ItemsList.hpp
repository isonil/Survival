#ifndef APP_ITEMS_LIST_HPP
#define APP_ITEMS_LIST_HPP

#include "engine/util/DataFile.hpp"

namespace app
{

class ItemDef;

class ItemsList : public engine::DataFile::Saveable
{
public:
    class Element : public engine::DataFile::Saveable
    {
    public:
        Element();

        void expose(engine::DataFile::Node &node) override;

        ItemDef &getItemDef() const;
        const std::shared_ptr <ItemDef> &getItemDefPtr() const;
        int getStack() const;

    private:
        std::string m_itemDef_defName;
        std::shared_ptr <ItemDef> m_itemDef;
        int m_stack;
    };

    void expose(engine::DataFile::Node &node) override;

    const std::vector <Element> &getItems() const;

private:
    std::vector <Element> m_items;
};

} // namespace app

#endif // APP_ITEMS_LIST_HPP
