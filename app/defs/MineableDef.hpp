#ifndef APP_MINEABLE_DEF_HPP
#define APP_MINEABLE_DEF_HPP

#include "parts/ItemsListWithUnboundedStack.hpp"
#include "parts/ItemsList.hpp"
#include "EntityDef.hpp"

#include <memory>

namespace engine { namespace app3D { class ModelDef; } }

namespace app
{

class MineableDef : public EntityDef, public engine::Tracked <MineableDef>
{
public:
    MineableDef();

    void expose(engine::DataFile::Node &node) override;

    const std::shared_ptr <engine::app3D::ModelDef> &getRandomModelDefPtr() const;
    const ItemsListWithUnboundedStack &getResources() const;
    const ItemsList &getItemsToSpawnWhenDestroyed() const;
    bool hasMineableTag(const std::string &mineableTag) const;
    int getInitialDurability() const;

private:
    using base = EntityDef;

    std::vector <std::string> m_modelDefs_names;
    std::vector <std::shared_ptr <engine::app3D::ModelDef>> m_modelDefs;
    ItemsListWithUnboundedStack m_resources;
    ItemsList m_itemsToSpawnWhenDestroyed;
    std::vector <std::string> m_mineableTags;
    int m_initialDurability;
};

} // namespace app

#endif // APP_MINEABLE_DEF_HPP
