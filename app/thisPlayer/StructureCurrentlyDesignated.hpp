#ifndef APP_STRUCTURE_CURRENTLY_DESIGNATED_HPP
#define APP_STRUCTURE_CURRENTLY_DESIGNATED_HPP

#include "engine/util/Vec3.hpp"
#include "engine/util/Color.hpp"

namespace engine { namespace app3D { class Model; class CollisionDetector; } }

namespace app
{

class StructureRecipeDef;
class StructureDef;
class Structure;

class StructureCurrentlyDesignated
{
public:
    void update();
    void draw3D();

    bool hasStructure() const;
    void setStructureRecipeDef(const std::shared_ptr <StructureRecipeDef> &recipe);
    void removeStructure();
    bool tryBuild();

    StructureRecipeDef &getStructureRecipeDef() const;
    const std::shared_ptr <StructureRecipeDef> &getStructureRecipeDefPtr() const;

private:
    struct Placement
    {
        Placement();
        Placement(bool canPlace, const engine::FloatVec3 &pos, const engine::FloatVec3 &rot);

        bool canPlace;
        engine::FloatVec3 pos;
        engine::FloatVec3 rot;
    };

    void updateModelAndCollisionDetector();
    Placement getDesignatedPlacement();
    Placement getDesignatedPlacement_rayHit(const engine::FloatVec3 &rayHitPos, int hitEntityID, const engine::FloatVec3 &defaultRot);
    Placement getDesignatedPlacement_rayNotHit(const engine::FloatVec3 &designatedPos, const engine::FloatVec3 &defaultRot);
    std::shared_ptr <Structure> getElectricityConnection();
    void addModelAndCollisionDetector();

    static const float k_distanceFromCameraToDesignatedPos;
    static const float k_minDistBetweenEntities;
    static const engine::Color k_canPlaceColor;
    static const engine::Color k_cantPlaceColor;

    std::shared_ptr <StructureRecipeDef> m_recipe;
    std::shared_ptr <engine::app3D::Model> m_model;
    std::shared_ptr <engine::app3D::CollisionDetector> m_collisionDetector;
};

} // namespace app

#endif // APP_STRUCTURE_CURRENTLY_DESIGNATED_HPP
