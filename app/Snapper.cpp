#include "Snapper.hpp"

#include "entities/Structure.hpp"
#include "defs/StructureDef.hpp"
#include "defs/CachedCollisionShapeDef.hpp"
#include "engine/app3D/IrrlichtConversions.hpp"

#include <irrlicht/irrlicht.h>

namespace app
{

std::vector <std::pair <engine::FloatVec3, engine::FloatVec3>> Snapper::trySnap(const Structure &first, const StructureDef &secondDef, const engine::FloatVec3 &designatedPos)
{
    TRACK;

    const auto &firstCachedCollisionShapeDef = first.getDef().getCachedCollisionShapeDef();
    const auto &secondCachedCollisionShapeDef = secondDef.getCachedCollisionShapeDef();

    // currently snapping works only for boxes

    if(firstCachedCollisionShapeDef.getType() != CachedCollisionShapeDef::Type::Box ||
       secondCachedCollisionShapeDef.getType() != CachedCollisionShapeDef::Type::Box)
        return {};

    const auto &firstSize = firstCachedCollisionShapeDef.getBoxSize();
    const auto &secondSize = secondCachedCollisionShapeDef.getBoxSize();

    const auto &secondPosOffset = secondCachedCollisionShapeDef.getPosOffset();

    bool firstIsFloor{firstSize.y < std::min(firstSize.x, firstSize.z)};
    bool secondIsFloor{secondSize.y < std::min(secondSize.x, secondSize.z)};

    if(firstIsFloor && secondIsFloor)
        return floorToFloor(firstSize, secondSize, designatedPos, first, secondPosOffset);
    else if(!firstIsFloor && !secondIsFloor)
        return wallToWall(firstSize, secondSize, designatedPos, first, secondPosOffset);
    else if(!firstIsFloor && secondIsFloor)
        return floorToWall(firstSize, secondSize, designatedPos, first, secondPosOffset);
    else
        return wallToFloor(firstSize, secondSize, designatedPos, first, secondPosOffset);
}

/*   Relative pos:
 *
*    /\  /
*    |  / Z
*   Y| /
*    |/
*     -------->
*         X
*
*  Note: physical bodies are centered (center of the box is at 0, 0, 0) (before applying offsets)
*/

std::vector <std::pair <engine::FloatVec3, engine::FloatVec3>> Snapper::floorToFloor(const engine::FloatVec3 &firstSize, const engine::FloatVec3 &secondSize, const engine::FloatVec3 &designatedPos, const Structure &first, const engine::FloatVec3 &secondPosOffset)
{
    TRACK;

    /*           F
     *       ---------
     *      /        /
     *   L /        / R
     *    /        /
     *    --------
     *       N
     */

    std::array <engine::FloatVec3, 4> hotspots{{
        {-firstSize.x / 2.f, 0.f, 0.f}, // left
        {firstSize.x / 2.f, 0.f, 0.f}, // right
        {0.f, 0.f, firstSize.z / 2.f}, // far
        {0.f, 0.f, -firstSize.z / 2.f} // near
    }};

    const auto &inWorldRot = first.getInWorldRotation();
    const auto &bestMany = getBestHotspots(designatedPos, first, hotspots);

    std::vector <std::pair <engine::FloatVec3, engine::FloatVec3>> ret;

    for(const auto &best : bestMany) {
        engine::FloatVec3 pos;

        switch(best) {
        case 0:
            pos = hotspots[best].movedX(-secondSize.x / 2.f);
            break;

        case 1:
            pos = hotspots[best].movedX(secondSize.x / 2.f);
            break;

        case 2:
            pos = hotspots[best].movedZ(secondSize.z / 2.f);
            break;

        default:
            pos = hotspots[best].movedZ(-secondSize.z / 2.f);
            break;
        }

        ret.push_back(std::make_pair(relPosToWorldPos(pos - secondPosOffset, first), inWorldRot));
    }

    return ret;
}

std::vector <std::pair <engine::FloatVec3, engine::FloatVec3>> Snapper::wallToWall(const engine::FloatVec3 &firstSize, const engine::FloatVec3 &secondSize, const engine::FloatVec3 &designatedPos, const Structure &first, const engine::FloatVec3 &secondPosOffset)
{
    TRACK;

    /*    ----T----
     *   /--------/|
     *   |        ||
     *  L| FL  FR ||R
     *   |        ||
     *   |        ||
     *    --------/
     *        B
     */

    std::array <engine::FloatVec3, 8> hotspots{{
        {-firstSize.x / 2.f, 0.f, 0.f}, // left
        {firstSize.x / 2.f, 0.f, 0.f}, // right
        {0.f, firstSize.y / 2.f, 0.f}, // top
        {0.f, -firstSize.y / 2.f, 0.f}, // bottom
        {-firstSize.x / 10.f, 0.f, -firstSize.z / 2.f}, // front left
        {firstSize.x / 10.f, 0.f, -firstSize.z / 2.f}, // front right
        {-firstSize.x / 10.f, 0.f, firstSize.z / 2.f}, // back left
        {firstSize.x / 10.f, 0.f, firstSize.z / 2.f} // back right
    }};

    const auto &inWorldRot = first.getInWorldRotation();
    const auto &bestMany = getBestHotspots(designatedPos, first, hotspots);

    std::vector <std::pair <engine::FloatVec3, engine::FloatVec3>> ret;

    irr::core::matrix4 inWorldRotMat;
    inWorldRotMat.setRotationDegrees(engine::app3D::IrrlichtConversions::toVector(inWorldRot));

    for(const auto &best : bestMany) {
        engine::FloatVec3 pos;
        irr::core::vector3df rot;

        switch(best) {
        case 0:
            pos = hotspots[best].movedX(-secondSize.x / 2.f);
            break;

        case 1:
            pos = hotspots[best].movedX(secondSize.x / 2.f);
            break;

        case 2:
            pos = hotspots[best].movedY(secondSize.y / 2.f);
            break;

        case 3:
            pos = hotspots[best].movedY(-secondSize.y / 2.f);
            break;

        case 4:
            pos = hotspots[0].moved(secondSize.z / 2.f, 0.f, (-secondSize.x - firstSize.z) / 2.f);
            rot.Y = 270.f;
            break;

        case 5:
            pos = hotspots[1].moved(-secondSize.z / 2.f, 0.f, (-secondSize.x - firstSize.z) / 2.f);
            rot.Y = 90.f;
            break;

        case 6:
            pos = hotspots[0].moved(secondSize.z / 2.f, 0.f, (secondSize.x + firstSize.z) / 2.f);
            rot.Y = 270.f;
            break;

        default:
            pos = hotspots[1].moved(-secondSize.z / 2.f, 0.f, (secondSize.x + firstSize.z) / 2.f);
            rot.Y = 90.f;
            break;
        }

        irr::core::matrix4 relRotMat;
        relRotMat.setRotationDegrees(rot);

        const auto &finRot = (inWorldRotMat * relRotMat).getRotationDegrees();

        ret.push_back(std::make_pair(relPosToWorldPos(pos - secondPosOffset, first), engine::FloatVec3{finRot.X, finRot.Y, finRot.Z}));
    }

    return ret;
}

std::vector <std::pair <engine::FloatVec3, engine::FloatVec3>> Snapper::floorToWall(const engine::FloatVec3 &firstSize, const engine::FloatVec3 &secondSize, const engine::FloatVec3 &designatedPos, const Structure &first, const engine::FloatVec3 &secondPosOffset)
{
    TRACK;

    /*    ----BT---
     *   /---FT---/|
     *   |        ||
     *   |        ||
     *   |        ||
     *   |        ||
     *    ---FB---/
     */

    std::array <engine::FloatVec3, 4> hotspots{{
        {0.f, firstSize.y / 2.f, -firstSize.z / 2.f}, // front top
        {0.f, -firstSize.y / 2.f, -firstSize.z / 2.f}, // front bottom
        {0.f, firstSize.y / 2.f, firstSize.z / 2.f}, // back top
        {0.f, -firstSize.y / 2.f, firstSize.z / 2.f}, // back bottom
    }};

    const auto &inWorldRot = first.getInWorldRotation();
    const auto &bestMany = getBestHotspots(designatedPos, first, hotspots);

    std::vector <std::pair <engine::FloatVec3, engine::FloatVec3>> ret;

    irr::core::matrix4 inWorldRotMat;
    inWorldRotMat.setRotationDegrees(engine::app3D::IrrlichtConversions::toVector(inWorldRot));

    for(const auto &best : bestMany) {
        engine::FloatVec3 pos;
        irr::core::vector3df rot;

        switch(best) {
        case 0:
            pos = hotspots[best].moved(0.f, -secondSize.y / 2.f, -secondSize.z / 2.f);
            break;

        case 1:
            pos = hotspots[best].moved(0.f, secondSize.y / 2.f, -secondSize.z / 2.f);
            break;

        case 2:
            pos = hotspots[best].moved(0.f, -secondSize.y / 2.f, secondSize.z / 2.f);
            break;

        default:
            pos = hotspots[best].moved(0.f, secondSize.y / 2.f, secondSize.z / 2.f);
            break;
        }

        irr::core::matrix4 relRotMat;
        relRotMat.setRotationDegrees(rot);

        const auto &finRot = (inWorldRotMat * relRotMat).getRotationDegrees();

        ret.push_back(std::make_pair(relPosToWorldPos(pos - secondPosOffset, first), engine::FloatVec3{finRot.X, finRot.Y, finRot.Z}));
    }

    return ret;
}

std::vector <std::pair <engine::FloatVec3, engine::FloatVec3>> Snapper::wallToFloor(const engine::FloatVec3 &firstSize, const engine::FloatVec3 &secondSize, const engine::FloatVec3 &designatedPos, const Structure &first, const engine::FloatVec3 &secondPosOffset)
{
    TRACK;

    /*
     *       ----TF---
     *      /        //
     *     /TL    TR//
     *    /        //BR
     *   |----TN--//
     *    ---BN---/
     */

    std::array <engine::FloatVec3, 8> hotspots{{
        {-firstSize.x / 2.f, firstSize.y / 2.f, 0.f}, // top left
        {firstSize.x / 2.f, firstSize.y / 2.f, 0.f}, // top right
        {0.f, firstSize.y / 2.f, firstSize.z / 2.f}, // top far
        {0.f, firstSize.y / 2.f, -firstSize.z / 2.f}, // top near
        {-firstSize.x / 2.f, -firstSize.y / 2.f, 0.f}, // bottom left
        {firstSize.x / 2.f, -firstSize.y / 2.f, 0.f}, // bottom right
        {0.f, -firstSize.y / 2.f, firstSize.z / 2.f}, // bottom far
        {0.f, -firstSize.y / 2.f, -firstSize.z / 2.f} // bottom near
    }};

    const auto &inWorldRot = first.getInWorldRotation();
    const auto &bestMany = getBestHotspots(designatedPos, first, hotspots);

    std::vector <std::pair <engine::FloatVec3, engine::FloatVec3>> ret;

    irr::core::matrix4 inWorldRotMat;
    inWorldRotMat.setRotationDegrees(engine::app3D::IrrlichtConversions::toVector(inWorldRot));

    for(const auto &best : bestMany) {
        engine::FloatVec3 pos;
        irr::core::vector3df rot;

        switch(best) {
        case 0:
            pos = hotspots[best].moved(secondSize.z / 2.f, secondSize.y / 2.f, 0.f);
            rot.Y = 270.f;
            break;

        case 1:
            pos = hotspots[best].moved(-secondSize.z / 2.f, secondSize.y / 2.f, 0.f);
            rot.Y = 90.f;
            break;

        case 2:
            pos = hotspots[best].moved(0.f, secondSize.y / 2.f, -secondSize.z / 2.f);
            break;

        case 3:
            pos = hotspots[best].moved(0.f, secondSize.y / 2.f, secondSize.z / 2.f);
            rot.Y = 180.f;
            break;

        case 4:
            pos = hotspots[best].moved(secondSize.z / 2.f, -secondSize.y / 2.f, 0.f);
            rot.Y = 270.f;
            break;

        case 5:
            pos = hotspots[best].moved(-secondSize.z / 2.f, -secondSize.y / 2.f, 0.f);
            rot.Y = 90.f;
            break;

        case 6:
            pos = hotspots[best].moved(0.f, -secondSize.y / 2.f, -secondSize.z / 2.f);
            break;

        default:
            pos = hotspots[best].moved(0.f, -secondSize.y / 2.f, secondSize.z / 2.f);
            rot.Y = 180.f;
            break;
        }

        irr::core::matrix4 relRotMat;
        relRotMat.setRotationDegrees(rot);

        const auto &finRot = (inWorldRotMat * relRotMat).getRotationDegrees();

        ret.push_back(std::make_pair(relPosToWorldPos(pos - secondPosOffset, first), engine::FloatVec3{finRot.X, finRot.Y, finRot.Z}));
    }

    return ret;
}

engine::FloatVec3 Snapper::relPosToWorldPos(const engine::FloatVec3 &relPos, const Structure &structure)
{
    TRACK;

    irr::core::matrix4 rotationMatrix;

    const auto &rot = engine::app3D::IrrlichtConversions::toVector(structure.getInWorldRotation());
    auto pos = engine::app3D::IrrlichtConversions::toVector(relPos + structure.getDef().getCachedCollisionShapeDef().getPosOffset());

    rotationMatrix.setRotationDegrees(rot);
    rotationMatrix.rotateVect(pos);

    const auto &inWorldPos = engine::app3D::IrrlichtConversions::toVector(structure.getInWorldPosition());
    const auto &finPos = pos + inWorldPos;

    return {finPos.X, finPos.Y, finPos.Z};
}

} // namespace app
