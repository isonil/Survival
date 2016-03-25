#ifndef ENGINE_APP_3D_VERTICES_AND_INDICES_NODE_HPP
#define ENGINE_APP_3D_VERTICES_AND_INDICES_NODE_HPP

#include <irrlicht.h>

#include <vector>

namespace engine
{
namespace app3D
{
namespace irrNodes
{

class VerticesAndIndicesNode : public irr::scene::ISceneNode
{
public:
    VerticesAndIndicesNode(const std::vector <irr::video::S3DVertex> &vertices, std::vector <int> &indices, irr::scene::ISceneManager &sceneManager);

    void OnRegisterSceneNode() override;
    void render() override;
    const irr::core::aabbox3d <irr::f32> &getBoundingBox() const override;
    irr::u32 getMaterialCount() const override;
    irr::video::SMaterial &getMaterial(irr::u32 i) override;

private:
    irr::scene::SMeshBuffer m_meshBuffer;
    irr::core::aabbox3d <irr::f32> m_boundingBox;
    irr::video::SMaterial m_material;
};

} // namespace irrNodes
} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_VERTICES_AND_INDICES_NODE_HPP
