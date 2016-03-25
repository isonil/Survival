#include "Music.hpp"

#include "LogManager.hpp"
#include "AppTime.hpp"
#include "Math.hpp"

namespace engine
{

Music::Music(const std::string &path, bool relativeToListener, bool loop)
    : m_fadeInOutVolumeFactor{1.f},
      m_fadeIn{true}
{
    if(!m_music.openFromFile(path)) {
        E_ERROR("Could not load music \"%s\".", path.c_str());
    }

    m_music.setRelativeToListener(relativeToListener);
    m_music.setLoop(loop);
}

void Music::update(const AppTime &appTime)
{
    if(m_fadeIn) {
        if(!Math::fuzzyCompare(m_fadeInOutVolumeFactor, 1.f)) {
            m_fadeInOutVolumeFactor += appTime.getDeltaAsSeconds() * k_volumeChangeSpeed;

            m_fadeInOutVolumeFactor = Math::clamp01(m_fadeInOutVolumeFactor);

            if(Math::fuzzyCompare(m_fadeInOutVolumeFactor, 1.f))
                m_fadeInOutVolumeFactor = 1.f;

            m_music.setVolume(m_fadeInOutVolumeFactor * 100.f);
        }
    }
    else {
        if(!Math::fuzzyCompare(m_fadeInOutVolumeFactor, 0.f)) {
            m_fadeInOutVolumeFactor -= appTime.getDeltaAsSeconds() * k_volumeChangeSpeed;

            m_fadeInOutVolumeFactor = Math::clamp01(m_fadeInOutVolumeFactor);

            if(Math::fuzzyCompare(m_fadeInOutVolumeFactor, 0.f)) {
                m_fadeInOutVolumeFactor = 0.f;
                m_music.setVolume(0.f);
                m_music.stop();
            }
            else
                m_music.setVolume(m_fadeInOutVolumeFactor * 100.f);
        }
    }
}

void Music::play()
{
    m_fadeIn = true;
    m_fadeInOutVolumeFactor = 1.f;
    m_music.play();
}

void Music::stop()
{
    m_music.stop();
}

bool Music::stopped() const
{
    return m_music.getStatus() == sf::Music::Status::Stopped;
}

void Music::fadeIn()
{
    m_fadeIn = true;
    m_fadeInOutVolumeFactor = 0.f;
    m_music.play();
}

void Music::fadeOut()
{
    m_fadeIn = false;
}

void Music::setPosition(const FloatVec3 &pos)
{
    m_music.setPosition(pos.x, pos.y, pos.z);
}

void Music::setAttenuation(float attenuation)
{
    m_music.setAttenuation(attenuation);
}

void Music::setMinDistance(float minDistance)
{
    m_music.setMinDistance(minDistance);
}

const float Music::k_volumeChangeSpeed{0.5f};

} // namespace engine
