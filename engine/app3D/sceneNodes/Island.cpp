#include "Island.hpp"

#include "../managers/ResourcesManager.hpp"
#include "../irrNodes/VerticesAndIndicesNode.hpp"
#include "../Device.hpp"
#include "../IrrlichtConversions.hpp"

namespace engine
{
namespace app3D
{

Island::Island(const std::vector <irr::video::S3DVertex> &vertices, const std::vector <int> &indices, const std::weak_ptr <Device> &device)
    : SceneNode{device},
      m_currentRender{},
      m_vertices{vertices},
      m_indices{indices}
{
    createRender();
}

void Island::dropIrrObjects()
{
    m_currentRender.node = nullptr;
}

void Island::reloadIrrObjects()
{
    createRender();
}

bool Island::wantsEverUpdate() const
{
    return false;
}

void Island::setPosition(const FloatVec3 &pos)
{
    if(pos != m_pos) {
        m_pos = pos;
        updateCurrentRenderPosition();
    }
}

std::vector <FloatVec3> Island::getTriangles() const
{
    std::vector <FloatVec3> ret;

    for(auto index : m_indices) {
        E_DASSERT(index >= 0 && index < static_cast <int> (m_vertices.size()), "Index out of bounds.");
        ret.push_back({m_vertices[index].Pos.X, m_vertices[index].Pos.Y, m_vertices[index].Pos.Z});
    }

    return ret;
}

Island::~Island()
{
    TRACK;

    if(!deviceExpired())
        removeCurrentRender();
}

void Island::createRender()
{
    removeCurrentRender();

    auto &device = getDevice_slow();
    auto &rm = device.getResourcesManager();
    auto &irrSceneManager = *device.getIrrDevice().getSceneManager();

    m_currentRender.node = new irrNodes::VerticesAndIndicesNode{m_vertices, m_indices, irrSceneManager};
    m_currentRender.node->setMaterialTexture(0, &rm.loadIrrTexture("terrain/grass.jpg", true));

    updateCurrentRenderPosition();
}

void Island::removeCurrentRender()
{
    if(m_currentRender.node) {
        m_currentRender.node->remove();
        m_currentRender.node = nullptr;
    }
}

void Island::updateCurrentRenderPosition()
{
    if(m_currentRender.node)
        m_currentRender.node->setPosition(IrrlichtConversions::toVector(m_pos));
}

} // namespace app3D
} // namespace engine
