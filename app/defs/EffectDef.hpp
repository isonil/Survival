#ifndef APP_EFFECT_DEF_HPP
#define APP_EFFECT_DEF_HPP

#include "engine/util/Def.hpp"

namespace engine { namespace app3D { class ParticlesGroupDef; class SoundDef; class LightDef; } }

namespace app
{

class EffectDef : public engine::Def
{
public:
    class ParticleEffect : public engine::DataFile::Saveable
    {
    public:
        ParticleEffect();

        void expose(engine::DataFile::Node &node) override;

        engine::app3D::ParticlesGroupDef &getParticlesGroupDef() const;
        const std::shared_ptr <engine::app3D::ParticlesGroupDef> &getParticlesGroupDefPtr() const;

        float getStartOffset() const;
        float getDuration() const;
        bool lastsForever() const;

    private:
        std::string m_particlesGroupDef_defName;
        std::shared_ptr <engine::app3D::ParticlesGroupDef> m_particlesGroupDef;

        float m_startOffset;
        float m_duration;
        bool m_lastsForever;
    };

    void expose(engine::DataFile::Node &node) override;

    const std::vector <ParticleEffect> &getParticleEffects() const;

    bool hasSoundDef() const;
    engine::app3D::SoundDef &getSoundDef() const;
    const std::shared_ptr <engine::app3D::SoundDef> &getSoundDefPtr() const;

    bool hasLightDef() const;
    const std::shared_ptr <engine::app3D::LightDef> &getLightDefPtr() const;

    bool lastsForever() const;

private:
    using base = Def;

    std::vector <ParticleEffect> m_particleEffects;
    std::string m_soundDef_defName;
    std::shared_ptr <engine::app3D::SoundDef> m_soundDef;
    std::string m_lightDef_defName;
    std::shared_ptr <engine::app3D::LightDef> m_lightDef;
};

} // namespace app

#endif // APP_EFFECT_DEF_HPP
