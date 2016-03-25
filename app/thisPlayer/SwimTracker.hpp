#ifndef APP_SWIM_TRACKER_HPP
#define APP_SWIM_TRACKER_HPP

#include "engine/util/Music.hpp"

namespace app
{

class SwimTracker
{
public:
    SwimTracker();

    void update();

private:
    static const std::string k_underWaterAmbiencePath;

    bool m_previouslyUnderWater;
    engine::Music m_underWaterAmbience;
};

} // namespace app

#endif // APP_SWIM_TRACKER_HPP

