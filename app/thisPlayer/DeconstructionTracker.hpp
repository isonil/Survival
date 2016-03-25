#ifndef APP_DECONSTRUCTION_TRACKER_HPP
#define APP_DECONSTRUCTION_TRACKER_HPP

#include "../util/Timer.hpp"

#include <irrlicht/Keycodes.h>

namespace app
{

class Entity;

class DeconstructionTracker
{
public:
    void update();

    void startDeconstructing(const std::weak_ptr <Entity> &entity);
    void stopDeconstructing();
    bool isDeconstructingAnything() const;
    float getDeconstructionProgress() const;

    static constexpr irr::EKEY_CODE k_deconstructKey{irr::KEY_KEY_X};

private:
    static const float k_deconstructionDuration;

    Timer m_deconstructionTimer;
    std::weak_ptr <Entity> m_deconstructedEntity;
};

} // namespace app

#endif // APP_DECONSTRUCTION_TRACKER_HPP
