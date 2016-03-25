#ifndef APP_EFFECT_HPP
#define APP_EFFECT_HPP

#include "engine/util/Vec3.hpp"

namespace engine { namespace app3D { class Sound; class ParticlesGroup; class ParticlesGroupDef; class Light; } }

namespace app
{

class EffectDef;

class Effect
{
public:
    Effect(const std::shared_ptr <EffectDef> &def, const engine::FloatVec3 &pos, const engine::FloatVec3 &rot);

    Effect(Effect &&) noexcept = default;

    Effect &operator = (Effect &&) noexcept = default;

    void update();

    void setPosition(const engine::FloatVec3 &pos);
    void setRotation(const engine::FloatVec3 &rot);

    void setAllCurrentZonesToBox(const engine::FloatVec3 &dimension);
    void setAllCurrentZonesToSphere(float radius);

    void stop();
    bool ended() const;

    ~Effect();

private:
    struct CurrentParticlesGroup
    {
        std::shared_ptr <engine::app3D::ParticlesGroup> particlesGroup;
        double endTime{};
        bool lastsForever{};
    };

    struct PendingParticlesGroup
    {
        std::shared_ptr <engine::app3D::ParticlesGroupDef> particlesGroupDef;
        double startTime{};
        double endTime{};
        bool lastsForever{};
    };

    std::shared_ptr <EffectDef> m_def;

    std::vector <CurrentParticlesGroup> m_currentParticlesGroups;
    std::vector <PendingParticlesGroup> m_pendingParticlesGroups;

    engine::FloatVec3 m_pos;
    engine::FloatVec3 m_rot;

    std::unique_ptr <engine::app3D::Sound> m_sound;
    std::shared_ptr <engine::app3D::Light> m_light;

    double m_startTime;
};

} // namespace app

#endif // APP_EFFECT_HPP
