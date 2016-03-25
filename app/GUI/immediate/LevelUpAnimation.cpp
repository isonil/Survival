#include "LevelUpAnimation.hpp"

#include "engine/GUI/GUIManager.hpp"
#include "engine/GUI/IGUIRenderer.hpp"
#include "engine/GUI/IGUITexture.hpp"
#include "engine/app3D/Device.hpp"
#include "../../Global.hpp"
#include "../../Core.hpp"

namespace app
{

LevelUpAnimation::LevelUpAnimation()
    : m_pos{0.f, InterpolationType::FixedStepLinear, k_posInterpolationStep},
      m_alpha{0.f, InterpolationType::FixedStepLinear, k_alphaInterpolationStep},
      m_alphaDecrease{0.f, InterpolationType::FixedStepLinear, k_alphaDecreaseInterpolationStep},
      m_reachedMaxAlpha{}
{
    auto &GUIRenderer = Global::getCore().getDevice().getGUIManager().getRenderer();

    m_levelUpTexture = GUIRenderer.getTexture(k_levelUpTexturePath);
}

void LevelUpAnimation::show()
{
    m_pos.setValueWithoutInterpolation(-3.f);
    m_pos.setTargetValue(15.f);
    m_alpha.setValueWithoutInterpolation(0.f);
    m_alpha.setTargetValue(1.f);

    m_lifeSpanTimer.set(k_lifeSpan);
    m_reachedMaxAlpha = false;
}

void LevelUpAnimation::update()
{
    m_pos.update();
    m_alpha.update();
    m_alphaDecrease.update();

    if(!m_reachedMaxAlpha && engine::Math::fuzzyCompare(m_alpha.getCurrentValue(), 1.f)) {
        m_reachedMaxAlpha = true;
        m_alphaDecrease.setValueWithoutInterpolation(1.f);
    }

    if(m_reachedMaxAlpha && m_lifeSpanTimer.passed())
        m_alphaDecrease.setTargetValue(0.f);
}

void LevelUpAnimation::draw() const
{
    float alpha{m_reachedMaxAlpha ? m_alphaDecrease.getCurrentValue() : m_alpha.getCurrentValue()};

    if(!engine::Math::fuzzyCompare(alpha, 0.f)) {
        E_DASSERT(m_levelUpTexture, "Texture is nullptr.");

        const auto &screenSize = Global::getCore().getDevice().getScreenSize();

        engine::IntVec2 pos{static_cast <int> (m_pos.getCurrentValue()),
                            static_cast <int> (0.2f * screenSize.y)};

        m_levelUpTexture->draw(pos, {1.f, 1.f, 1.f, alpha});
    }
}

const std::string LevelUpAnimation::k_levelUpTexturePath = "GUI/levelUp.png";
const float LevelUpAnimation::k_posInterpolationStep{16.f};
const float LevelUpAnimation::k_alphaInterpolationStep{4.f};
const float LevelUpAnimation::k_alphaDecreaseInterpolationStep{0.3f};
const float LevelUpAnimation::k_lifeSpan{5000.f};

} // namespace app
