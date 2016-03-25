#include "GUITexture.hpp"

#include "../../util/Exception.hpp"
#include "../../util/Color.hpp"
#include "../managers/ResourcesManager.hpp"
#include "../Device.hpp"
#include "../IrrlichtConversions.hpp"

namespace engine
{
namespace app3D
{
namespace detail
{

GUITexture::GUITexture(const std::string &path, const std::weak_ptr <Device> &device)
    : m_device{device},
      m_texture{},
      m_texturePath{path}
{
    TRACK;

    if(m_device.expired())
        throw Exception{"Device is nullptr."};

    if(m_texturePath.empty())
        throw Exception{"Tried to load texture using empty path."};

    reloadIrrObjects();
}

void GUITexture::dropIrrObjects()
{
    m_texture = nullptr;
}

void GUITexture::reloadIrrObjects()
{
    m_texture = &getDevice_slow().getResourcesManager().loadIrrTexture(m_texturePath, true);
}

void GUITexture::draw(const IntRect &rect, const Color &color) const
{
    TRACK;

    E_DASSERT(m_texture, "Texture is nullptr.");

    auto &drv = *getDevice_slow().getIrrDevice().getVideoDriver();

    const auto &irrRect = IrrlichtConversions::toRect(rect);
    const auto &irrColor = IrrlichtConversions::toColor(color);

    irr::core::recti fullRect{0, 0,
                              static_cast <irr::s32> (m_texture->getSize().Width),
                              static_cast <irr::s32> (m_texture->getSize().Height)};

    irr::video::SColor irrColors[4]{irrColor, irrColor, irrColor, irrColor};

    // TODO: figure out which is better, with material2D or without; same in GUIRenderer

    //drv.enableMaterial2D();
    drv.draw2DImage(m_texture, irrRect, fullRect, nullptr, irrColors, true);
    //drv.enableMaterial2D(false);
}

IntVec2 GUITexture::getSize() const
{
    E_DASSERT(m_texture, "Texture is nullptr.");

    const auto &size = m_texture->getSize();

    return {static_cast <int> (size.Width),
            static_cast <int> (size.Height)};
}

Device &GUITexture::getDevice_slow() const
{
    const auto &shared = m_device.lock();

    E_DASSERT(shared, "Device is nullptr.");

    return *shared;
}

} // namespace detail
} // namespace app3D
} // namespace engine
