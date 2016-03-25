#ifndef APP_REVIVING_TRACKER_HPP
#define APP_REVIVING_TRACKER_HPP

#include "../util/Timer.hpp"

#include <irrlicht/Keycodes.h>

namespace app
{

class Entity;

class RevivingTracker
{
public:
    void update();

    void startReviving(const std::weak_ptr <Entity> &entity);
    void stopReviving();
    bool isRevivingAnything() const;
    float getRevivingProgress() const;

    static constexpr irr::EKEY_CODE k_reviveKey{irr::KEY_KEY_E};

private:
    static const float k_revivingDuration;

    Timer m_revivingTimer;
    std::weak_ptr <Entity> m_revivedEntity;
};

} // namespace app

#endif // APP_REVIVING_TRACKER_HPP
