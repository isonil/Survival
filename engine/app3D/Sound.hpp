#ifndef ENGINE_APP_3D_SOUND_HPP
#define ENGINE_APP_3D_SOUND_HPP

#include "../util/Trace.hpp"
#include "../util/Vec3.hpp"

#include <SFML/Audio.hpp>

#include <memory>

namespace engine { class AppTime; }

namespace engine
{
namespace app3D
{

class SoundDef;

class Sound : public Tracked <Sound>
{
public:
    explicit Sound(const std::shared_ptr <SoundDef> &soundDef);

    Sound(Sound &&) = default;

    Sound &operator = (Sound &&) = default;

    void update(const AppTime &appTime);

    void play();
    void playLooped();
    void stop();

    bool stopped() const;

    void fadeIn();
    void fadeInLooped();
    void fadeOut();

    void setPosition(const FloatVec3 &pos);

    ~Sound();

private:
    static const float k_volumeChangeSpeed;

    void createSoundInstance();

    std::shared_ptr <SoundDef> m_soundDef;
    // we use std::unique_ptr <sf::Sound> because there are only limited sf::Sound instances available
    std::unique_ptr <sf::Sound> m_sound; // sound must be destroyed before sound def
    bool m_fadeIn;
    float m_fadeInOutVolumeFactor;
    float m_volume;
    FloatVec3 m_pos;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_SOUND_HPP
