#include "ProgressBar.hpp"

#include "../../util/Color.hpp"
#include "../IGUIRenderer.hpp"

namespace engine
{
namespace GUI
{

ProgressBar::ProgressBar(WidgetContainer *parent, const std::shared_ptr <IGUIRenderer> &renderer, const IntRect &rect)
    : Widget{parent, renderer, rect},
      m_progress{}
{
}

void ProgressBar::update(const AppTime &time, bool nonOccludedMouseOver)
{
}

void ProgressBar::draw() const
{
    const auto &GUIRenderer = getRenderer();
    const auto &screenRect = getScreenRect();
    const auto &progressRect = screenRect.movedRightBorder(-(1.f - m_progress) * screenRect.size.x);

    Color leftBot{0.3f, 0.3f, 0.6f};
    Color rightBot{0.4f, 0.4f, 0.8f};
    const auto &leftTop = leftBot.brightened(0.1f);
    const auto &rightTop = rightBot.brightened(0.1f);
    Color gray{0.7f, 0.7f, 0.7f};

    GUIRenderer.drawGradientRect(progressRect, leftBot, rightBot, rightBot, leftBot);
    GUIRenderer.drawGradientRect(progressRect.movedBottomBorder(-progressRect.size.y / 2), leftTop, rightTop, rightTop, leftTop);
    GUIRenderer.drawRectOutline(screenRect, gray);

    //GUIRenderer.drawText(std::to_string(static_cast <int> (m_progress * 100.f)) + " %", screenRect, gray, IGUIRenderer::FontSize::Normal, IGUIRenderer::HorizontalTextAlign::Center, IGUIRenderer::VerticalTextAlign::Middle);
}

bool ProgressBar::anyNonInternalChildWidgets() const
{
    return false;
}

void ProgressBar::setProgress(float progress)
{
    m_progress = Math::clamp01(progress);
}

} // namespace GUI
} // namespace engine
