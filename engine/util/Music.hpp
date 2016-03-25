#ifndef ENGINE_MUSIC_HPP
#define ENGINE_MUSIC_HPP

#include "Trace.hpp"
#include "Vec3.hpp"

#include <SFML/Audio.hpp>

namespace engine
{

class AppTime;

class Music : public Tracked <Music>
{
public:
    explicit Music(const std::string &path, bool relativeToListener = true, bool loop = true);

    void update(const AppTime &appTime);

    void play();
    void stop();

    bool stopped() const;

    void fadeIn();
    void fadeOut();

    void setPosition(const FloatVec3 &pos);
    void setAttenuation(float attenuation);
    void setMinDistance(float minDistance);

private:
    static const float k_volumeChangeSpeed;

    sf::Music m_music;
    float m_fadeInOutVolumeFactor;
    bool m_fadeIn;
};

} // namespace engine

#endif // ENGINE_MUSIC_HPP

