#include "BloodSplat.hpp"

#include "../../Global.hpp"
#include "../../Core.hpp"
#include "engine/util/Math.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/GUI/GUIManager.hpp"
#include "engine/GUI/IGUIRenderer.hpp"
#include "engine/GUI/IGUITexture.hpp"

namespace app
{

BloodSplat::BloodSplat()
    : m_reachedFullAlpha{},
      m_lifeSpanTimer{k_lifeSpan},
      m_alphaIncrease{0.f, InterpolationType::FixedStepLinear, k_alphaIncreaseStep},
      m_alphaDecrease{1.f, InterpolationType::FixedStepLinear, k_alphaDecreaseStep},
      m_stretchFactor{0.f, InterpolationType::FixedStepLinear, k_stretchFactorStep}
{
    m_alphaIncrease.setTargetValue(1.f);
    m_stretchFactor.setTargetValue(0.3f);

    auto &device = Global::getCore().getDevice();
    const auto &screenSize = device.getScreenSize();

    engine::IntVec2 center{screenSize.x / 4 + engine::Random::rangeInclusive(0, screenSize.x / 2),
                           screenSize.y / 4 + engine::Random::rangeInclusive(0, screenSize.y / 2)};

    E_DASSERT(!k_texturePaths.empty(), "There are no textures.");

    int index{engine::Random::rangeExclusive(0, k_texturePaths.size())};
    m_texture = device.getGUIManager().getRenderer().getTexture(k_texturePaths[index]);

    float scaleFactor{static_cast <float> (screenSize.x) / m_texture->getSize().x * 0.7f};

    const auto &textureSize = m_texture->getSize();

    engine::IntVec2 size{static_cast <int> (textureSize.x * scaleFactor),
                         static_cast <int> (textureSize.y * scaleFactor)};

    m_rect = {center - size / 2, size};
}

void BloodSplat::update()
{
    m_alphaIncrease.update();
    m_alphaDecrease.update();
    m_stretchFactor.update();

    if(engine::Math::fuzzyCompare(m_alphaIncrease.getCurrentValue(), 1.f))
        m_reachedFullAlpha = true;

    if(m_reachedFullAlpha && m_lifeSpanTimer.passed())
        m_alphaDecrease.setTargetValue(0.f);
}

void BloodSplat::draw() const
{
    E_DASSERT(m_texture, "Texture is nullptr.");

    float alpha{};

    if(m_reachedFullAlpha)
        alpha = m_alphaDecrease.getCurrentValue();
    else
        alpha = m_alphaIncrease.getCurrentValue();

    auto sizeY = static_cast <int> (m_rect.size.y + m_rect.size.y * m_stretchFactor.getCurrentValue());

    m_texture->draw({m_rect.pos, {m_rect.size.x, sizeY}}, {1.f, 1.f, 1.f, alpha});
}

bool BloodSplat::wantsToBeRemoved() const
{
    return m_lifeSpanTimer.passed() && engine::Math::fuzzyCompare(m_alphaDecrease.getCurrentValue(), 0.f);
}

const float BloodSplat::k_lifeSpan{3000.f};
const float BloodSplat::k_alphaIncreaseStep{7.f};
const float BloodSplat::k_alphaDecreaseStep{0.3f};
const float BloodSplat::k_stretchFactorStep{0.03f};
const std::vector <std::string> BloodSplat::k_texturePaths = {
    "GUI/bloodSplat1.png",
    "GUI/bloodSplat2.png",
    "GUI/bloodSplat3.png"
};

} // namespace app
