#ifndef APP_EXPERIENCE_BAR_HPP
#define APP_EXPERIENCE_BAR_HPP

#include "engine/util/Vec2.hpp"
#include "engine/util/Trace.hpp"

namespace app
{

class ExperienceBar : public engine::Tracked <ExperienceBar>
{
public:
    ExperienceBar();

    void draw() const;

    void setPosition(const engine::IntVec2 &pos);
    void setProgress(float progress);
    void setTitle(const std::string &title);
    void setLevel(int level);

private:
    engine::IntVec2 m_pos;
    float m_progress;
    std::string m_title;
    int m_level;
};

} // namespace app

#endif // APP_EXPERIENCE_BAR_HPP

