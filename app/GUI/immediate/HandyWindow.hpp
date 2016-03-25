#ifndef APP_HANDY_WINDOW_HPP
#define APP_HANDY_WINDOW_HPP

#include "engine/util/Trace.hpp"
#include "engine/util/Vec2.hpp"

namespace engine { namespace GUI { class Event; class IGUITexture; } }

namespace app
{

class HandyWindow : public engine::Tracked <HandyWindow>
{
public:
    enum class Type
    {
        Construct,
        SkillsAndUpgrades,
        Inventory
    };

    explicit HandyWindow(const engine::IntVec2 &size);

    virtual void update();
    virtual void onEvent(engine::GUI::Event &event);
    virtual void draw();
    virtual Type getType() const = 0;

    virtual ~HandyWindow() = default;

private:
    static const std::string k_shadowPath;

    engine::IntVec2 m_size;
    std::shared_ptr <engine::GUI::IGUITexture> m_shadow;
};

} // namespace app

#endif // APP_HANDY_WINDOW_HPP
