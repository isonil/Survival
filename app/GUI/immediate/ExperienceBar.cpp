#include "ExperienceBar.hpp"

#include "../../Global.hpp"
#include "../../Core.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/util/Rect.hpp"
#include "engine/GUI/IGUIRenderer.hpp"
#include "engine/GUI/GUIManager.hpp"

namespace app
{

ExperienceBar::ExperienceBar()
    : m_progress{},
      m_level{}
{
}

void ExperienceBar::draw() const
{
    const auto &GUIRenderer = Global::getCore().getDevice().getGUIManager().getRenderer();

    engine::IntRect rect{m_pos, {260, 18}};

    GUIRenderer.drawFilledRect(rect, {51, 82, 152, 100});

    auto perc = static_cast <int> (rect.size.x * m_progress);

    GUIRenderer.drawFilledRect(rect.movedRightBorder(-(rect.size.x - perc)), {67, 109, 205});
    GUIRenderer.drawLine({rect.pos.x + perc, rect.pos.y}, {rect.pos.x + perc, rect.pos.y + rect.size.y}, {166, 166, 166});
    GUIRenderer.drawLine({rect.pos.x, rect.pos.y + rect.size.y}, {rect.pos.x + rect.size.x, rect.pos.y + rect.size.y}, {22, 22, 22});
    GUIRenderer.drawLine({rect.pos.x + rect.size.x, rect.pos.y}, {rect.pos.x + rect.size.x, rect.pos.y + rect.size.y}, {22, 22, 22});

    GUIRenderer.drawText(m_title, rect.pos.moved(-15, -25), engine::Color::k_white);
    GUIRenderer.drawText(std::to_string(m_level), {rect.pos.movedX(-30), {25, rect.size.y}}, engine::Color::k_white, engine::GUI::IGUIRenderer::FontSize::Normal, engine::GUI::IGUIRenderer::HorizontalTextAlign::Right, engine::GUI::IGUIRenderer::VerticalTextAlign::Middle);
}

void ExperienceBar::setPosition(const engine::IntVec2 &pos)
{
    m_pos = pos;
}

void ExperienceBar::setProgress(float progress)
{
    m_progress = progress;
}

void ExperienceBar::setTitle(const std::string &title)
{
    m_title = title;
}

void ExperienceBar::setLevel(int level)
{
    m_level = level;
}

} // namespace app
