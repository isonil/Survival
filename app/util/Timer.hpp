#ifndef APP_TIMER_HPP
#define APP_TIMER_HPP

#include "engine/util/DataFile.hpp"

namespace app
{

class Timer : public engine::DataFile::Saveable
{
public:
    Timer();
    explicit Timer(double duration);

    void expose(engine::DataFile::Node &node) override;

    void set(double duration);
    bool passed() const;
    float getProgress() const;

private:
    double m_startTime;
    double m_endTime;
};

} // namespace app

#endif // APP_TIMER_HPP
