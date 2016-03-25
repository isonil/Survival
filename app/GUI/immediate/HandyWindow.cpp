#include "HandyWindow.hpp"

#include "engine/app3D/Device.hpp"
#include "engine/GUI/Event.hpp"
#include "engine/GUI/GUIManager.hpp"
#include "engine/GUI/IGUITexture.hpp"
#include "engine/GUI/IGUIRenderer.hpp"
#include "../../Core.hpp"
#include "../../Global.hpp"

namespace app
{

HandyWindow::HandyWindow(const engine::IntVec2 &size)
    : m_size{size}
{
    auto &GUIRenderer = Global::getCore().getDevice().getGUIManager().getRenderer();

    m_shadow = GUIRenderer.getTexture(k_shadowPath);
}

void HandyWindow::update()
{
}

void HandyWindow::onEvent(engine::GUI::Event &event)
{
    if(event.isSubstantial())
        event.absorb();
}

void HandyWindow::draw()
{
    TRACK;

    E_DASSERT(m_shadow, "Shadow is nullptr.");

    auto &device = Global::getCore().getDevice();
    auto &GUIRenderer = device.getGUIManager().getRenderer();
    const auto &screenSize = device.getScreenSize();

    engine::IntVec2 texSize{static_cast <int> (m_size.x * 1.34f),
                            static_cast <int> (m_size.y * 1.23f)};

    m_shadow->draw({screenSize - texSize, texSize}, engine::Color::k_black);

    GUIRenderer.drawLine({screenSize.x - m_size.x, screenSize.y - m_size.y + 20},
                         {screenSize.x, screenSize.y - m_size.y + 20},
                         {1.f, 1.f, 1.f, 0.4f});
}

const std::string HandyWindow::k_shadowPath = "GUI/shadow2.png";

} // namespace app
