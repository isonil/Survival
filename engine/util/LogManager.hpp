#ifndef ENGINE_LOG_MANAGER_HPP
#define ENGINE_LOG_MANAGER_HPP

#include <fstream>
#include <sstream>
#include <string>

#define DEBUG_TARGET

#ifdef DEBUG_TARGET
#  define E_INFO(x...) ::engine::LogManager::info(__PRETTY_FUNCTION__, __LINE__, x);
#  define E_ERROR(x...) ::engine::LogManager::error(__PRETTY_FUNCTION__, __LINE__, x);
#  define E_WARNING(x...) ::engine::LogManager::warning(__PRETTY_FUNCTION__, __LINE__, x);
#  define E_DEBUG(x...) ::engine::LogManager::debug(__PRETTY_FUNCTION__, __LINE__, x);
#  define E_DASSERT(cond, x...)                                                          \
       if(!(cond)) {                                                                     \
           ::engine::LogManager::dassert(__PRETTY_FUNCTION__, __LINE__, #cond, x);       \
       }
#  define E_LOG_TAG(tag, x...) ::engine::LogManager::custom(__PRETTY_FUNCTION__, __LINE__, tag, x);
#else
#  define E_INFO(x...) ::engine::LogManager::info(nullptr, 0, x);
#  define E_ERROR(x...) ::engine::LogManager::error(nullptr, 0, x);
#  define E_WARNING(x...) ::engine::LogManager::warning(nullptr, 0, x);
#  define E_DEBUG(x...)
#  define E_DASSERT(x...)
#  define E_LOG_TAG(tag, x...) ::engine::LogManager::custom(nullptr, 0, tag, x);
#endif

#define E_RASSERT(cond, x...)                                                         \
    if(!(cond)) {                                                                     \
        ::engine::LogManager::rassert(__PRETTY_FUNCTION__, __LINE__, #cond, x);       \
    }
#define E_LOG(x...) ::engine::LogManager::custom(nullptr, 0, nullptr, x);

namespace engine
{

class AppInfo;

// TODO: don't use char*
// TODO: use variadic templates or 3rd party lib like cppformat
class LogManager
{
public:
    LogManager(const LogManager &) = delete;

    LogManager &operator = (const LogManager &) = delete;

    static void create(const AppInfo &appInfo);
    static void info(const char *functionName, int line, const char *format, ...);
    static void error(const char *functionName, int line, const char *format, ...);
    static void warning(const char *functionName, int line, const char *format, ...);
    static void debug(const char *functionName, int line, const char *format, ...);
    static void dassert(const char *functionName, int line, const char *expression, const char *format, ...);
    static void rassert(const char *functionName, int line, const char *expression, const char *format, ...);
    static void custom(const char *functionName, int line, const char *tag, const char *format, ...);
    static std::string getLog();

private:
    static void log(const char *functionName,
                    int line,
                    const char *expression,
                    const char *msgBoxDesc,
                    bool isCritical,
                    const char *tag,
                    const char *text);

    static const size_t k_maxLogEntrySize;
    static const std::string k_logFilePath;

    static std::ofstream m_logFile;
    static std::ostringstream m_log;
};

} // namespace engine

#endif // ENGINE_LOG_MANAGER_HPP
