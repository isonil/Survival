#ifndef APP_SOUND_POOL_HPP
#define APP_SOUND_POOL_HPP

#include "engine/app3D/Sound.hpp"

#include <deque>

namespace app
{

class Entity;

class SoundPool
{
public:
    void update();

    void play(const std::shared_ptr <engine::app3D::SoundDef> &soundDef);
    void play(const std::shared_ptr <engine::app3D::SoundDef> &soundDef, const engine::FloatVec3 &pos);
    void play(const std::shared_ptr <engine::app3D::SoundDef> &soundDef, const std::weak_ptr <Entity> &entity);

private:
    struct SoundWithInfo
    {
        SoundWithInfo(engine::app3D::Sound &&sound);
        SoundWithInfo(engine::app3D::Sound &&sound, const std::weak_ptr <Entity> &entity);

        engine::app3D::Sound sound;
        bool followEntity;
        std::weak_ptr <Entity> entity;
    };

    std::list <SoundWithInfo> m_sounds;
};

} // namespace app

#endif // APP_SOUND_POOL_HPP
