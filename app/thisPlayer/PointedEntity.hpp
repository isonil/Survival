#ifndef APP_POINTED_ENTITY_HPP
#define APP_POINTED_ENTITY_HPP

#include <memory>

namespace app
{

class Entity;

class PointedEntity
{
public:
    void update();

    bool isAny() const;
    Entity &getEntity() const;
    std::shared_ptr <Entity> getEntityPtr() const;

private:
    static const float k_rayLengthToPickEntity;

    std::weak_ptr <Entity> m_pointedEntity;
};

} // namespace app

#endif // APP_POINTED_ENTITY_HPP
