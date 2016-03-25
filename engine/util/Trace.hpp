#ifndef ENGINE_TRACE_HPP
#define ENGINE_TRACE_HPP

#include <QElapsedTimer>

#include <chrono>
#include <string>
#include <vector>
#include <typeinfo>

#define TRACE_USE_TRACKERS // comment to disable
#define TRACE_PROFILE // comment to disable

#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

#ifdef TRACE_USE_TRACKERS
#  define TRACK                                                                                              \
       static int TRACK_trackerID{};                                                                         \
       static bool TRACK_initialize{true};                                                                   \
       if(unlikely(TRACK_initialize)) {                                                                      \
           TRACK_initialize = false;                                                                         \
           TRACK_trackerID = ::engine::Trace::allTrackers.size();                                            \
           ::engine::Trace::allTrackers.push_back( { __PRETTY_FUNCTION__, __FILE__, __func__, 0.f } );       \
       }                                                                                                     \
       ::engine::Trace::Tracker TRACK_tracker{TRACK_trackerID};                                              \
       do {} while(0)
#else
#  define TRACK do {} while(0)
#endif

namespace engine
{

class Trace
{
public:
    class Tracker
    {
    public:
        explicit Tracker(int ID);
        Tracker(const Tracker &) = delete;

        Tracker &operator = (const Tracker &) = delete;

        ~Tracker();

    private:
        void startProfile();
        void endProfile();

#ifdef TRACE_PROFILE
        int m_trackerID;
        qint64 m_startNsecs;
#endif
    };

    struct TrackerInfo
    {
        std::string function;
        std::string file;
        std::string shortFunction;
        float totalMsWorkingVar; // in milliseconds
    };

    struct ClassInfo
    {
        std::string name;
        int livingObjectsCount = {};
    };

    static void initProfiler();
    static void profilerLap();
    static std::string getTrace();
    static void checkMemoryLeaks();
    static std::string getProfilerResults();
    static qint64 getElapsedNsecs();

    // exception to encapsulation rule for speed (probably not needed anyway)
    static std::vector <TrackerInfo> allTrackers;
    static std::vector <ClassInfo> allClasses;

private:
    static constexpr size_t k_maxActiveTrackers{200};
    static constexpr size_t k_maxTrackersPerFrame{750000};
    static constexpr qint64 k_minNsecsToShowInHistory{1000000};

    static QElapsedTimer m_elapsedTimer;
    static int m_activeTrackers[k_maxActiveTrackers];
    static size_t m_activeTrackersIndex;

#ifdef TRACE_PROFILE
    static int m_currentTrackersDepth;
    static size_t m_trackersFrameHistoryCurrentIndex;
    static int m_trackersFrameHistoryTrackerID[k_maxTrackersPerFrame];
    static int m_trackersFrameHistoryDepth[k_maxTrackersPerFrame];
    static qint64 m_trackersFrameHistoryNsecsElapsed[k_maxTrackersPerFrame];
#endif
};

template <typename T> class Tracked
{
public:
    Tracked();
    Tracked(const Tracked &);
    Tracked(Tracked &&);

    Tracked &operator = (const Tracked &) = default;
    Tracked &operator = (Tracked &&) = default;

    ~Tracked();

private:
    void constructed();

    static int m_classID;
    static bool m_initialized;
};

inline Trace::Tracker::Tracker(int ID)
{
    if(likely(m_activeTrackersIndex < k_maxActiveTrackers))
        m_activeTrackers[m_activeTrackersIndex] = ID;
    ++m_activeTrackersIndex;

#ifdef TRACE_PROFILE
    m_trackerID = ID;
    startProfile();
#endif
}

inline Trace::Tracker::~Tracker()
{
    --m_activeTrackersIndex;

#ifdef TRACE_PROFILE
    endProfile();
#endif
}

inline void Trace::Tracker::startProfile()
{
#ifdef TRACE_PROFILE
    ++m_currentTrackersDepth;
    m_startNsecs = getElapsedNsecs();
#endif
}

inline void Trace::Tracker::endProfile()
{
#ifdef TRACE_PROFILE
    --m_currentTrackersDepth;
    if(likely(m_trackersFrameHistoryCurrentIndex < k_maxTrackersPerFrame)) {
        m_trackersFrameHistoryTrackerID[m_trackersFrameHistoryCurrentIndex] = m_trackerID;
        m_trackersFrameHistoryDepth[m_trackersFrameHistoryCurrentIndex] = m_currentTrackersDepth;
        m_trackersFrameHistoryNsecsElapsed[m_trackersFrameHistoryCurrentIndex] = getElapsedNsecs() - m_startNsecs;
    }
    ++m_trackersFrameHistoryCurrentIndex;
#endif
}

inline qint64 Trace::getElapsedNsecs()
{
    return m_elapsedTimer.nsecsElapsed();
}

template <typename T> Tracked <T>::Tracked()
{
    constructed();
}

template <typename T> Tracked <T>::Tracked(const Tracked &)
{
    constructed();
}

template <typename T> Tracked <T>::Tracked(Tracked &&)
{
    constructed();
}

template <typename T> Tracked <T>::~Tracked()
{
    --Trace::allClasses[m_classID].livingObjectsCount;
}

template <typename T> inline void Tracked <T>::constructed()
{
    if(unlikely(!m_initialized)) {
        m_initialized = true;
        m_classID = -1;

        const std::string &typeName{typeid(T).name()};

        for(size_t i = 0; i < Trace::allClasses.size(); ++i) {
            if(Trace::allClasses[i].name == typeName)
                m_classID = i;
        }

        if(m_classID < 0) {
            Trace::allClasses.resize(Trace::allClasses.size() + 1);
            Trace::allClasses.back().name = typeName;
            m_classID = Trace::allClasses.size() - 1;
        }
    }

    ++Trace::allClasses[m_classID].livingObjectsCount;
}

template <typename T> int Tracked <T>::m_classID{-1};
template <typename T> bool Tracked <T>::m_initialized{};

} // namespace engine

#endif // ENGINE_TRACE_HPP
