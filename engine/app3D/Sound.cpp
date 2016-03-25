#include "Sound.hpp"

#include "../util/AppTime.hpp"
#include "defs/SoundDef.hpp"

namespace engine
{
namespace app3D
{

Sound::Sound(const std::shared_ptr <SoundDef> &soundDef)
    : m_soundDef{soundDef},
      m_fadeIn{true},
      m_fadeInOutVolumeFactor{1.f},
      m_volume{1.f}
{
    if(!m_soundDef)
        throw Exception{"Sound def is nullptr."};
}

void Sound::update(const AppTime &appTime)
{
    if(!m_sound)
        return;

    if(m_fadeIn) {
        if(!Math::fuzzyCompare(m_fadeInOutVolumeFactor, 1.f)) {
            m_fadeInOutVolumeFactor += appTime.getDeltaAsSeconds() * k_volumeChangeSpeed;

            m_fadeInOutVolumeFactor = Math::clamp01(m_fadeInOutVolumeFactor);

            if(Math::fuzzyCompare(m_fadeInOutVolumeFactor, 1.f))
                m_fadeInOutVolumeFactor = 1.f;

            m_sound->setVolume(m_volume * m_fadeInOutVolumeFactor);
        }
    }
    else {
        if(!Math::fuzzyCompare(m_fadeInOutVolumeFactor, 0.f)) {
            m_fadeInOutVolumeFactor -= appTime.getDeltaAsSeconds() * k_volumeChangeSpeed;

            m_fadeInOutVolumeFactor = Math::clamp01(m_fadeInOutVolumeFactor);

            if(Math::fuzzyCompare(m_fadeInOutVolumeFactor, 0.f)) {
                m_fadeInOutVolumeFactor = 0.f;
                m_sound->setVolume(0.f);
                m_sound->stop();
                m_sound.reset();
            }
            else
                m_sound->setVolume(m_volume * m_fadeInOutVolumeFactor);
        }
    }
}

void Sound::play()
{
    createSoundInstance();

    E_DASSERT(m_sound, "Sound not created.");
    E_DASSERT(m_soundDef, "Sound def is nullptr.");

    m_sound->setLoop(false);

    m_fadeIn = true;
    m_fadeInOutVolumeFactor = 1.f;

    m_volume = m_soundDef->getVolumeRandomRange().randomElement() * 100.f;
    m_sound->setVolume(m_volume);
    m_sound->setPitch(m_soundDef->getPitchRandomRange().randomElement());
    m_sound->play();
}

void Sound::playLooped()
{
    createSoundInstance();

    E_DASSERT(m_sound, "Sound not created.");
    E_DASSERT(m_soundDef, "Sound def is nullptr.");

    m_sound->setLoop(true);

    m_fadeIn = true;
    m_fadeInOutVolumeFactor = 1.f;

    m_volume = m_soundDef->getVolumeRandomRange().randomElement() * 100.f;
    m_sound->setVolume(m_volume);
    m_sound->setPitch(m_soundDef->getPitchRandomRange().randomElement());
    m_sound->play();
}

void Sound::stop()
{
    if(m_sound) {
        m_sound->stop();
        m_sound.reset();
    }
}

bool Sound::stopped() const
{
    return !m_sound || m_sound->getStatus() == sf::Sound::Status::Stopped;
}

void Sound::fadeIn()
{
    createSoundInstance();

    E_DASSERT(m_sound, "Sound not created.");
    E_DASSERT(m_soundDef, "Sound def is nullptr.");

    m_sound->setLoop(false);

    m_fadeIn = true;
    m_fadeInOutVolumeFactor = 0.f;

    m_volume = m_soundDef->getVolumeRandomRange().randomElement() * 100.f;
    m_sound->setVolume(m_volume);
    m_sound->setPitch(m_soundDef->getPitchRandomRange().randomElement());
    m_sound->play();
}

void Sound::fadeInLooped()
{
    createSoundInstance();

    E_DASSERT(m_sound, "Sound not created.");
    E_DASSERT(m_soundDef, "Sound def is nullptr.");

    m_sound->setLoop(true);

    m_fadeIn = true;
    m_fadeInOutVolumeFactor = 0.f;

    m_volume = m_soundDef->getVolumeRandomRange().randomElement() * 100.f;
    m_sound->setVolume(m_volume);
    m_sound->setPitch(m_soundDef->getPitchRandomRange().randomElement());
    m_sound->play();
}

void Sound::fadeOut()
{
    m_fadeIn = false;
}

void Sound::setPosition(const FloatVec3 &pos)
{
    m_pos = pos;

    if(m_sound)
        m_sound->setPosition(pos.x, pos.y, pos.z);
}

Sound::~Sound()
{
    if(m_sound) {
        m_sound->stop();
        m_sound->resetBuffer();
        m_sound.reset();
    }
}

const float Sound::k_volumeChangeSpeed{3.5f};

void Sound::createSoundInstance()
{
    if(m_sound)
        return;

    m_sound = std::make_unique <sf::Sound> ();

    E_DASSERT(m_soundDef, "Sound def is nullptr.");

    m_sound->setBuffer(m_soundDef->getSoundBuffer());
    m_sound->setRelativeToListener(m_soundDef->isGUISound());
    m_sound->setAttenuation(m_soundDef->getAttenuation());
    m_sound->setMinDistance(m_soundDef->getMinDistance());
    m_sound->setPosition(m_pos.x, m_pos.y, m_pos.z);
}

} // namespace app3D
} // namespace engine
