#ifndef ENGINE_APP_3D_DETAIL_GUI_TEXTURE_HPP
#define ENGINE_APP_3D_DETAIL_GUI_TEXTURE_HPP

#include "../../util/Trace.hpp"
#include "../../GUI/IGUITexture.hpp"
#include "../IIrrlichtObjectsHolder.hpp"

#include <irrlicht/irrlicht.h>

namespace engine { class Color; }
namespace engine { namespace app3D { class Device; } }

namespace engine
{
namespace app3D
{
namespace detail
{

class GUITexture : public GUI::IGUITexture, public IIrrlichtObjectsHolder, public Tracked <GUITexture>
{
public:
    GUITexture(const std::string &path, const std::weak_ptr <Device> &device);
    GUITexture(const GUITexture &) = delete;
    GUITexture(GUITexture &&) = default;

    GUITexture &operator = (const GUITexture &) = delete;
    GUITexture &operator = (GUITexture &&) = default;

    void dropIrrObjects() override;
    void reloadIrrObjects() override;

    void draw(const IntRect &rect, const Color &color = Color::k_white) const override;
    IntVec2 getSize() const override;

private:
    Device &getDevice_slow() const;

    std::weak_ptr <Device> m_device;
    irr::video::ITexture *m_texture;
    std::string m_texturePath;
};

} // namespace detail
} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_DETAIL_GUI_TEXTURE_HPP
