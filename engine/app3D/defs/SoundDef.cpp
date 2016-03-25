#include "SoundDef.hpp"

#include "../../util/Math.hpp"
#include "../managers/ResourcesManager.hpp"
#include "../Device.hpp"

namespace engine
{
namespace app3D
{

SoundDef::SoundDef()
    : m_isGUISound{true},
      m_attenuation{1.f},
      m_minDistance{3.f}
{
}

void SoundDef::expose(DataFile::Node &node)
{
    base::expose(node);

    node.var(m_path, "path");
    node.var(m_volumeRandomRange, "volumeRandomRange", {1.f, 1.f});
    node.var(m_pitchRandomRange, "pitchRandomRange", {1.f, 1.f});
    node.var(m_isGUISound, "isGUISound");
    node.var(m_attenuation, "attenuation", 1.f);
    node.var(m_minDistance, "minDistance", 3.f);

    if(node.getActivityType() == DataFile::Activity::Type::Loading) {
        if(m_volumeRandomRange.isEmpty())
            throw Exception{"Volume random range is empty."};

        if(m_pitchRandomRange.isEmpty())
            throw Exception{"Pitch random range is empty."};

        if(m_attenuation < 0.f)
            throw Exception{"Attenuation can't be negative."};

        if(Math::fuzzyCompare(m_minDistance, 0.f))
            throw Exception{"Minimum distance can't be 0. It would lead to an incorrect spatialization."};

        if(m_minDistance < 0.f)
            throw Exception{"Minimum distance can't be negative."};
    }
}

void SoundDef::dropIrrObjects()
{
}

void SoundDef::reloadIrrObjects()
{
    const auto &path = getDevice_slow().getResourcesManager().getPathToResource("sounds/" + m_path);

    if(!m_soundBuffer.loadFromFile(path)) {
        E_ERROR("Could not load sound \"%s\".", m_path.c_str());
    }
}

const sf::SoundBuffer &SoundDef::getSoundBuffer() const
{
    return m_soundBuffer;
}

const FloatRange &SoundDef::getVolumeRandomRange() const
{
    return m_volumeRandomRange;
}

const FloatRange &SoundDef::getPitchRandomRange() const
{
    return m_pitchRandomRange;
}

bool SoundDef::isGUISound() const
{
    return m_isGUISound;
}

float SoundDef::getAttenuation() const
{
    return m_attenuation;
}

float SoundDef::getMinDistance() const
{
    return m_minDistance;
}

} // namespace app3D
} // namespace engine
