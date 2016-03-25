#ifndef APP_BLOOD_SPLAT_HPP
#define APP_BLOOD_SPLAT_HPP

#include "../../util/Timer.hpp"
#include "../../util/InterpolatedFloat.hpp"
#include "engine/util/Trace.hpp"
#include "engine/util/Rect.hpp"

namespace engine { namespace GUI { class IGUITexture; } }

namespace app
{

class BloodSplat : public engine::Tracked <BloodSplat>
{
public:
    BloodSplat();

    void update();
    void draw() const;
    bool wantsToBeRemoved() const;

private:
    static const float k_lifeSpan;
    static const float k_alphaIncreaseStep;
    static const float k_alphaDecreaseStep;
    static const float k_stretchFactorStep;
    static const std::vector <std::string> k_texturePaths;

    bool m_reachedFullAlpha;
    Timer m_lifeSpanTimer;
    InterpolatedFloat m_alphaIncrease;
    InterpolatedFloat m_alphaDecrease;
    InterpolatedFloat m_stretchFactor;

    std::shared_ptr <engine::GUI::IGUITexture> m_texture;
    engine::IntRect m_rect;
};

} // namespace app

#endif // APP_BLOOD_SPLAT_HPP

