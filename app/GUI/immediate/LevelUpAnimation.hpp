#ifndef APP_LEVEL_UP_ANIMATION_HPP
#define APP_LEVEL_UP_ANIMATION_HPP

#include "../../util/InterpolatedFloat.hpp"
#include "../../util/Timer.hpp"

#include <string>
#include <memory>

namespace engine { namespace GUI { class IGUITexture; } }

namespace app
{

class LevelUpAnimation
{
public:
    LevelUpAnimation();

    void show();
    void update();
    void draw() const;

private:
    static const std::string k_levelUpTexturePath;
    static const float k_posInterpolationStep;
    static const float k_alphaInterpolationStep;
    static const float k_alphaDecreaseInterpolationStep;
    static const float k_lifeSpan;

    std::shared_ptr <engine::GUI::IGUITexture> m_levelUpTexture;

    InterpolatedFloat m_pos;
    InterpolatedFloat m_alpha;
    InterpolatedFloat m_alphaDecrease;
    Timer m_lifeSpanTimer;
    bool m_reachedMaxAlpha;
};

} // namespace app

#endif // APP_LEVEL_UP_ANIMATION_HPP

