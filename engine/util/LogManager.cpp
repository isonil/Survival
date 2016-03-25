#include "LogManager.hpp"

#include "../AppInfo.hpp"
#include "../EngineStaticInfo.hpp"
#include "Time.hpp"
#include "Trace.hpp"
#include "Version.hpp"
#include "Exception.hpp"

#include <QtWidgets/QMessageBox>
#include <QApplication>

#include <cstdarg>

namespace engine
{

void LogManager::create(const AppInfo &appInfo)
{
    TRACK;

    if(m_logFile.is_open()) {
        E_INFO("Closing log manager previous output file, creating new one.");
        m_logFile.close();
    }

    m_logFile.open(k_logFilePath);

    if(!m_logFile.is_open())
        throw Exception{"Could not create log manager output file \"" + k_logFilePath + "\"."};

    std::ostringstream initialText;

    initialText << "Log file created on " << Time::getLocal().toString()
                << std::endl;
    initialText << std::endl;
    initialText << "        App name  -  " << appInfo.getAppName() << std::endl;
    initialText << "  Engine version  -  " << EngineStaticInfo::k_engineVersion.toString() << std::endl;
    initialText << "     App version  -  " << appInfo.getAppVersion().toString() << std::endl;
    initialText << "       Copyright  -  " << appInfo.getAppCopyright() << std::endl;
    initialText << std::endl;
    initialText << "Application build information:" << std::endl;
    initialText << "       Date  -  " << __DATE__ << std::endl;
    initialText << "       Time  -  " << __TIME__ << std::endl;
    initialText << "     Target  -  ";

    #ifdef DEBUG_TARGET
    initialText << "Debug";
    #else
    initialText << "Release";
    #endif

    initialText << std::endl;

    const auto &initialTextStr = initialText.str();

    E_LOG("%s", initialTextStr.c_str());
}

void LogManager::info(const char *functionName, int line, const char *format, ...)
{
    TRACK;

    char text[k_maxLogEntrySize + 1];
    if(format) {
        va_list va;
        va_start(va, format);
        vsnprintf(text, k_maxLogEntrySize, format, va);
        va_end(va);
    }
    else text[0] = '\0';

    log(functionName, line, nullptr, nullptr, false, "info", text);
}

void LogManager::error(const char *functionName, int line, const char *format, ...)
{
    TRACK;

    char text[k_maxLogEntrySize + 1];
    if(format) {
        va_list va;
        va_start(va, format);
        vsnprintf(text, k_maxLogEntrySize, format, va);
        va_end(va);
    }
    else text[0] = '\0';

    log(functionName, line, nullptr, nullptr, false, "error", text);
}

void LogManager::warning(const char *functionName, int line, const char *format, ...)
{
    TRACK;

    char text[k_maxLogEntrySize + 1];
    if(format) {
        va_list va;
        va_start(va, format);
        vsnprintf(text, k_maxLogEntrySize, format, va);
        va_end(va);
    }
    else text[0] = '\0';

    log(functionName, line, nullptr, nullptr, false, "warning", text);
}

void LogManager::debug(const char *functionName, int line, const char *format, ...)
{
    TRACK;

    char text[k_maxLogEntrySize + 1];
    if(format) {
        va_list va;
        va_start(va, format);
        vsnprintf(text, k_maxLogEntrySize, format, va);
        va_end(va);
    }
    else text[0] = '\0';

    log(functionName, line, nullptr, nullptr, false, "debug", text);
}

void LogManager::dassert(const char *functionName, int line, const char *expression, const char *format, ...)
{
    TRACK;

    char text[k_maxLogEntrySize + 1];
    if(format) {
        va_list va;
        va_start(va, format);
        vsnprintf(text, k_maxLogEntrySize, format, va);
        va_end(va);
    }
    else text[0] = '\0';

    log(functionName, line, expression, "An assertion failed (debug).", true, "dassert", text);
}

void LogManager::rassert(const char *functionName, int line, const char *expression, const char *format, ...)
{
    TRACK;

    char text[k_maxLogEntrySize + 1];
    if(format) {
        va_list va;
        va_start(va, format);
        vsnprintf(text, k_maxLogEntrySize, format, va);
        va_end(va);
    }
    else text[0] = '\0';

    log(functionName, line, expression, "An assertion failed (release).", true, "rassert", text);
}

void LogManager::custom(const char *functionName, int line, const char *tag, const char *format, ...)
{
    TRACK;

    char text[k_maxLogEntrySize + 1];
    if(format) {
        va_list va;
        va_start(va, format);
        vsnprintf(text, k_maxLogEntrySize, format, va);
        va_end(va);
    }
    else text[0] = '\0';

    log(functionName, line, nullptr, nullptr, false, tag, text);
}

void LogManager::log(const char *functionName,
                     int line,
                     const char *expression,
                     const char *msgBoxDesc,
                     bool isCritical,
                     const char *tag,
                     const char *text)
{
    TRACK;

    std::ostringstream logText;

    if(tag)
        logText << '[' << tag << "] ";

    if(functionName)
        logText << '[' << functionName << ':' << line << "] ";

    if(expression)
        logText << "[expr: " << expression << "] ";

    if(text)
        logText << text;

    logText << std::endl;

    const auto &logTextStr = logText.str();

    printf("%s", logTextStr.c_str());
    fflush(stdout);

    if(m_logFile.is_open()) {
        m_logFile << logTextStr;
        m_logFile.flush();
    }

    m_log << logTextStr;
    m_log.flush();

    if(isCritical) {
        if(qApp) {
            std::ostringstream msgBoxText;

            if(msgBoxDesc)
                msgBoxText << msgBoxDesc;

            if(text) {
                if(!msgBoxText.str().empty())
                    msgBoxText << std::endl << std::endl;
                msgBoxText << text;
            }

            if(expression || functionName) {
                if(!msgBoxText.str().empty())
                    msgBoxText << std::endl << std::endl;

                if(expression)
                    msgBoxText << "Expression: " << expression << std::endl;

                if(functionName) {
                    msgBoxText << "Location: " << functionName << std::endl;
                    msgBoxText << "Line: " << line << std::endl;
                }
            }
            const auto &msgBoxTextStr = msgBoxText.str();

            QMessageBox::critical(nullptr, "Error", msgBoxTextStr.c_str());
        }
        exit(1);
    }
}

std::string LogManager::getLog()
{
    return m_log.str();
}

const size_t LogManager::k_maxLogEntrySize{2048};
const std::string LogManager::k_logFilePath = "log.txt";
std::ofstream LogManager::m_logFile;
std::ostringstream LogManager::m_log;

} // namespace engine
