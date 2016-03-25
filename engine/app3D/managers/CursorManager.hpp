#ifndef ENGINE_APP_3D_CURSOR_MANAGER_HPP
#define ENGINE_APP_3D_CURSOR_MANAGER_HPP

#include "../../util/Trace.hpp"
#include "../../util/Vec2.hpp"
#include "../IIrrlichtObjectsHolder.hpp"

#include <irrlicht/irrlicht.h>

namespace engine
{
namespace app3D
{

class Device;

class CursorManager : public IIrrlichtObjectsHolder, public Tracked <CursorManager>
{
public:
    CursorManager(Device &device);

    void showCursor(bool show = true);
    IntVec2 getCursorPosition() const;

    void dropIrrObjects() override;
    void reloadIrrObjects() override;

private:
    static const std::string k_normalCursorTexturePath;

    Device &m_device;

    IntVec2 m_cursorPosBeforeLocking;
    irr::gui::IGUISpriteBank *m_cursorSpriteBank;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_CURSOR_MANAGER_HPP
