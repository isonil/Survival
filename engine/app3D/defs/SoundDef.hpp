#ifndef ENGINE_APP_3D_SOUND_DEF_HPP
#define ENGINE_APP_3D_SOUND_DEF_HPP

#include "../../util/Range.hpp"
#include "ResourceDef.hpp"

#include <SFML/Audio.hpp>

namespace engine
{
namespace app3D
{

class SoundDef : public ResourceDef, public Tracked <SoundDef>
{
public:
    SoundDef();

    void expose(DataFile::Node &node) override;

    void dropIrrObjects() override;
    void reloadIrrObjects() override;

    const sf::SoundBuffer &getSoundBuffer() const;
    const FloatRange &getVolumeRandomRange() const;
    const FloatRange &getPitchRandomRange() const;
    bool isGUISound() const;
    float getAttenuation() const;
    float getMinDistance() const;

private:
    using base = ResourceDef;

    std::string m_path;
    sf::SoundBuffer m_soundBuffer;
    FloatRange m_volumeRandomRange;
    FloatRange m_pitchRandomRange;
    bool m_isGUISound;
    float m_attenuation;
    float m_minDistance;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_SOUND_DEF_HPP
