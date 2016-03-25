#include "VerticesAndIndicesNode.hpp"

namespace engine
{
namespace app3D
{
namespace irrNodes
{

VerticesAndIndicesNode::VerticesAndIndicesNode(const std::vector <irr::video::S3DVertex> &vertices, std::vector <int> &indices, irr::scene::ISceneManager &sceneManager)
    : irr::scene::ISceneNode(sceneManager.getRootSceneNode(), &sceneManager, -1)
{
    for(const auto &elem : vertices) {
        m_meshBuffer.Vertices.push_back(elem);
        m_boundingBox.addInternalPoint(elem.Pos);
    }

    for(auto index : indices) {
        m_meshBuffer.Indices.push_back(index);
    }

    sceneManager.getMeshManipulator()->recalculateNormals(&m_meshBuffer, false, false);

    m_material.GouraudShading = false;
    m_material.Lighting = true;
    m_material.FogEnable = true;
    m_material.DiffuseColor.set(255, 100, 100, 100);
}

void VerticesAndIndicesNode::OnRegisterSceneNode()
{
    if(IsVisible)
        SceneManager->registerNodeForRendering(this);

    ISceneNode::OnRegisterSceneNode();
}

void VerticesAndIndicesNode::render()
{
    auto *driver = SceneManager->getVideoDriver();

    driver->setMaterial(m_material);
    driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);
    driver->drawMeshBuffer(&m_meshBuffer);
}

const irr::core::aabbox3d <irr::f32> &VerticesAndIndicesNode::getBoundingBox() const
{
    return m_boundingBox;
}

irr::u32 VerticesAndIndicesNode::getMaterialCount() const
{
    return 1;
}

irr::video::SMaterial &VerticesAndIndicesNode::getMaterial(irr::u32 i)
{
    return m_material;
}

} // namespace irrNodes
} // namespace app3D
} // namespace engine
