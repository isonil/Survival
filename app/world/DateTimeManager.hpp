#ifndef APP_DATE_TIME_MANAGER_HPP
#define APP_DATE_TIME_MANAGER_HPP

#include "engine/util/Trace.hpp"
#include "engine/util/Time.hpp"

namespace engine { namespace app3D { class Light; } }

namespace app
{

class DateTimeManager : public engine::Tracked <DateTimeManager>
{
public:
    DateTimeManager();

    void update();
    const engine::Time &getTime() const;

private:
    void updateTime();
    void updateAmbientLight();
    void updateDirectionalLight();
    void updateFog();

    static const int k_initialGameYear;
    static const int k_initialHour;
    static const double k_gameTimeMultiplier;

    double m_startMs;
    double m_currentMs;
    engine::Time m_time;

    std::shared_ptr <engine::app3D::Light> m_directionalLight;
};

} // namespace app

#endif // APP_DATE_TIME_MANAGER_HPP
