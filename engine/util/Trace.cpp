#include "Trace.hpp"

#include "LogManager.hpp"

#include <utility>
#include <sstream>

namespace engine
{

void Trace::initProfiler()
{
#ifdef TRACE_PROFILE
    m_elapsedTimer.start();
#endif
}

void Trace::profilerLap()
{
#ifdef TRACE_PROFILE
    m_currentTrackersDepth = 0;
    m_trackersFrameHistoryCurrentIndex = 0;
#endif
}

std::string Trace::getTrace()
{
    std::ostringstream oss;

#ifdef TRACE_USE_TRACKERS
    for(int i = static_cast <int> (m_activeTrackersIndex) - 1; i >= 0; --i) {
        oss << allTrackers[m_activeTrackers[i]].file
            << ": "
            << allTrackers[m_activeTrackers[i]].function;

        if(i)
            oss << std::endl;
    }
#endif

    return oss.str();
}

void Trace::checkMemoryLeaks()
{
    E_INFO("Checking memory leaks. Registered classes: %d.", static_cast <int> (allClasses.size()));

    bool OK{true};

    for(const auto &elem : allClasses) {
        if(elem.livingObjectsCount < 0) {
            E_WARNING("Negative living objects count: \"%s\", objects count: %d.", elem.name.c_str(), elem.livingObjectsCount);
        }
        else if(elem.livingObjectsCount > 0) {
            E_WARNING("Memory leak: \"%s\", objects count: %d.", elem.name.c_str(), elem.livingObjectsCount);
            OK = false;
        }
    }

    if(OK) {
        E_INFO("Checking memory leaks done. Everything OK.");
    }
    else {
        E_INFO("Checking memory leaks done. Found leaks. Note that it can be static objects not destroyed yet.");
    }
}

std::string Trace::getProfilerResults()
{
    std::string res;

#ifdef TRACE_PROFILE
    res += "--- Call History ---\n\n";
    res += "History is in reverse order.\n";

    size_t currentIndex{m_trackersFrameHistoryCurrentIndex};

    if(m_trackersFrameHistoryCurrentIndex >= k_maxTrackersPerFrame) {
        res += "Trackers per frame count exceeded limit. Profiler results can be incomplete.\n";
        currentIndex = k_maxTrackersPerFrame;
    }

    std::vector <std::string> m_records;

    int minDepth{};
    bool first{};

    for(size_t i = 0; i < currentIndex; ++i) {
        if(m_trackersFrameHistoryNsecsElapsed[i] < k_minNsecsToShowInHistory)
            continue;

        if(first || m_trackersFrameHistoryDepth[i] < minDepth) {
            first = false;
            minDepth = m_trackersFrameHistoryDepth[i];
        }
    }

    for(size_t i = 0; i < currentIndex; ++i) {
        if(m_trackersFrameHistoryNsecsElapsed[i] < k_minNsecsToShowInHistory)
            continue;

        int curDepth{m_trackersFrameHistoryDepth[i] - minDepth};

        m_records.resize(m_records.size() + 1);
        for(int j = 0; j < curDepth; ++j) {
            m_records.back() += ' ';
        }

        int index{m_trackersFrameHistoryTrackerID[i]};

        E_DASSERT(index >= 0 && static_cast <size_t> (index) < allTrackers.size(), "Index out of bounds.");

        m_records.back() += allTrackers[index].function + ' ';

        auto nano = m_trackersFrameHistoryNsecsElapsed[i];
        auto micro = nano / 1000;

        m_records.back() += std::to_string(micro / 1000);
        m_records.back() += '.';

        if(micro % 1000 < 100)
            m_records.back() += '0';
        if(micro % 1000 < 10)
            m_records.back() += '0';

        m_records.back() += std::to_string(micro % 1000);
        m_records.back() += " ms";
    }

    for(size_t i = 0; i < m_records.size(); ++i) {
        res += m_records[i] + '\n';
    }

    res += "\n--- Total Time Taken ---\n\n";

    for(auto &elem : allTrackers) {
        elem.totalMsWorkingVar = 0.f;
    }

    for(size_t i = 0; i < currentIndex; ++i) {
        auto micro = m_trackersFrameHistoryNsecsElapsed[i] / 1000;
        float milli{micro / 1000.f};

        allTrackers[m_trackersFrameHistoryTrackerID[i]].totalMsWorkingVar += milli;
    }

    std::vector <std::pair <float, size_t>> total;

    for(size_t i = 0; i < allTrackers.size(); ++i) {
        if(allTrackers[i].totalMsWorkingVar > 0.f)
            total.push_back(std::make_pair(allTrackers[i].totalMsWorkingVar, i));
    }

    std::sort(total.begin(), total.end(), [](const auto &first, const auto &second) {
        return first.first > second.first;
    });

    for(const auto &elem : total) {
        res += allTrackers[elem.second].function + ' ';
        res += std::to_string(elem.first) + " ms\n";
    }

#endif

    return res;
}

std::vector <Trace::TrackerInfo> Trace::allTrackers;
std::vector <Trace::ClassInfo> Trace::allClasses;
QElapsedTimer Trace::m_elapsedTimer;
int Trace::m_activeTrackers[k_maxActiveTrackers]{};
size_t Trace::m_activeTrackersIndex{};

#ifdef TRACE_PROFILE
int Trace::m_currentTrackersDepth{};
size_t Trace::m_trackersFrameHistoryCurrentIndex{};
int Trace::m_trackersFrameHistoryTrackerID[k_maxTrackersPerFrame]{};
int Trace::m_trackersFrameHistoryDepth[k_maxTrackersPerFrame]{};
qint64 Trace::m_trackersFrameHistoryNsecsElapsed[k_maxTrackersPerFrame]{};
#endif

} // namespace engine
