#include "DateTimeManager.hpp"

#include "engine/app3D/managers/SceneManager.hpp"
#include "engine/app3D/sceneNodes/Light.hpp"
#include "../defs/DefsCache.hpp"
#include "engine/app3D/Device.hpp"
#include "../Global.hpp"
#include "../Core.hpp"
#include "WorldPart.hpp"

namespace app
{

DateTimeManager::DateTimeManager()
    : m_startMs{},
      m_currentMs{}
{
    auto &core = Global::getCore();
    auto &sceneManager = core.getDevice().getSceneManager();
    auto &defsCache = core.getDefsCache();

    m_startMs = core.getAppTime().getElapsedMs();
    m_directionalLight = sceneManager.addLight(defsCache.Light_Directional);

    update();
}

void DateTimeManager::update()
{
    TRACK;

    updateTime();
    updateAmbientLight();
    updateDirectionalLight();
    updateFog();
}

const engine::Time &DateTimeManager::getTime() const
{
    return m_time;
}

void DateTimeManager::updateTime()
{
    m_currentMs = Global::getCore().getAppTime().getElapsedMs();

    // int can be too small to hold full game seconds count
    auto fullSeconds = static_cast <long long> ((m_currentMs - m_startMs) * 0.001 * k_gameTimeMultiplier);

    // to avoid int overflow, we have to convert seconds to as many full days as possible
    auto days = fullSeconds / 86400;
    auto seconds = fullSeconds - days * 86400;

    // Time constructor will "normalize" time (so it's OK to pass, for example, 100 seconds, or 3582 days)
    m_time = engine::Time{k_initialGameYear, 0, static_cast <int> (days), k_initialHour, 0, static_cast <int> (seconds)};
}

void DateTimeManager::updateAmbientLight()
{
    auto &sceneManager = Global::getCore().getDevice().getSceneManager();

    static const std::vector <std::pair <float, float>> ambientLightValues = {
        std::make_pair(0.f, 0.07f),
        std::make_pair(7.f, 0.3f),
        std::make_pair(19.f, 0.3f),
    };

    float time{getTime().getHoursAsFloat()};

    sceneManager.setAmbientLight(engine::Math::lerpMany(ambientLightValues, 24.f, time));
}

void DateTimeManager::updateDirectionalLight()
{
    E_DASSERT(m_directionalLight, "Light is nullptr.");

    static const std::vector <std::pair <float, engine::Color>> lightColors = {
        std::make_pair <float, engine::Color> (3.f, {0.f, 0.f, 0.f}),
        std::make_pair <float, engine::Color> (7.f, {0.85f, 0.7f, 0.5f}), // (yellow)
        //std::make_pair <float, engine::Color> (7.f, {0.7f, 0.7f, 0.85f}), // (blue)
        std::make_pair <float, engine::Color> (11.f, {1.f, 1.f, 1.f}),
        std::make_pair <float, engine::Color> (17.f, {1.f, 1.f, 1.f}),
        std::make_pair <float, engine::Color> (19.f, {0.85f, 0.7f, 0.5f}),
        std::make_pair <float, engine::Color> (23.f, {0.f, 0.f, 0.f})
    };

    float time{getTime().getHoursAsFloat()};

    m_directionalLight->setColor(engine::Math::lerpMany(lightColors, 24.f, time));

    static const std::vector <std::pair <float, engine::FloatVec3>> lightDirections = {
        std::make_pair <float, engine::FloatVec3> (0.f, {1.f, -1.f, 0.3f}),
        std::make_pair <float, engine::FloatVec3> (12.f, {0.f, -1.f, 0.3f}),
        std::make_pair <float, engine::FloatVec3> (24.f, {-1.f, -1.f, 0.3f})
    };

    m_directionalLight->setDirectionalLightDirection(engine::Math::lerpMany(lightDirections, 24.f, time).normalized());
}

void DateTimeManager::updateFog()
{
    auto &device = Global::getCore().getDevice();
    const auto &camPos = device.getSceneManager().getCameraPosition();

    if(camPos.y < WorldPart::k_waterHeight)
        device.setFog(engine::Color{111, 166, 191}.darkened(30), 0.f, 90.f);
    else {
        static const std::vector <std::pair <float, engine::Color>> fogColors = {
            std::make_pair <float, engine::Color> (2.f, {0.1f, 0.1f, 0.1f}),
            std::make_pair <float, engine::Color> (5.f, {1.f, 0.7f, 0.32f}), // (yellow)
            //std::make_pair <float, engine::Color> (5.f, {0.7f, 0.7f, 0.9f}), // (blue)
            std::make_pair <float, engine::Color> (11.f, {1.f, 1.f, 1.f}),
            std::make_pair <float, engine::Color> (14.f, {1.f, 1.f, 1.f}),
            std::make_pair <float, engine::Color> (19.f, {1.f, 0.7f, 0.32f}),
            std::make_pair <float, engine::Color> (21.f, {0.1f, 0.1f, 0.1f})
        };

        static const std::vector <std::pair <float, float>> fogMinDists = {
            std::make_pair(0.f, 30.f),
            std::make_pair(6.f, 30.f),
            std::make_pair(12.f, 180.f),
            std::make_pair(18.f, 180.f),
            std::make_pair(23.f, 30.f)
        };

        static const std::vector <std::pair <float, float>> fogMaxDists = {
            std::make_pair(0.f, 300.f),
            std::make_pair(7.f, 300.f),
            std::make_pair(12.f, 500.f),
            std::make_pair(18.f, 500.f),
            std::make_pair(23.f, 500.f)
        };

        float time{getTime().getHoursAsFloat()};

        device.setFog(engine::Math::lerpMany(fogColors, 24.f, time),
                      engine::Math::lerpMany(fogMinDists, 24.f, time),
                      engine::Math::lerpMany(fogMaxDists, 24.f, time));
    }
}

const int DateTimeManager::k_initialGameYear{2500};
const int DateTimeManager::k_initialHour{6};
const double DateTimeManager::k_gameTimeMultiplier{60.0};

} // namespace app
