#ifndef ENGINE_APP_3D_ISLAND_HPP
#define ENGINE_APP_3D_ISLAND_HPP

#include "../../util/Vec3.hpp"
#include "SceneNode.hpp"

#include <irrlicht.h>

#include <vector>

namespace engine { namespace app3D { namespace irrNodes { class VerticesAndIndicesNode; } } }

namespace engine
{
namespace app3D
{

class Device;

// experimental class, not used anywhere
class Island : public SceneNode, public Tracked <Island>
{
public:
    Island(const std::vector <irr::video::S3DVertex> &vertices, const std::vector <int> &indices, const std::weak_ptr <Device> &device);

    void dropIrrObjects() override;
    void reloadIrrObjects() override;
    bool wantsEverUpdate() const;

    void setPosition(const FloatVec3 &pos);

    std::vector <FloatVec3> getTriangles() const;

    ~Island() override;

private:
    struct
    {
        irrNodes::VerticesAndIndicesNode *node{};
    } m_currentRender;

    void createRender();
    void removeCurrentRender();
    void updateCurrentRenderPosition();

    std::vector <irr::video::S3DVertex> m_vertices;
    std::vector <int> m_indices;
    FloatVec3 m_pos;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_ISLAND_HPP
