#include "App3D.hpp"

#include "app3D/Device.hpp"
#include "app3D/Settings.hpp"
#include "util/DefDatabase.hpp"
#include "util/Exception.hpp"
#include "util/LogManager.hpp"
#include "util/Trace.hpp"

#include <QApplication>

namespace engine
{

App3D::App3D()
    : m_qApp{}
{
}

void App3D::exec(int argc, char *argv[])
{
    TRACK;

    try {
        init(argc, argv);
        loop();
    }
    catch(const std::exception &e) {
        E_ERROR("Exception caught at App3D root level (closing): %s", e.what());
    }

    clean();
}

AppTime &App3D::getAppTime()
{
    return m_appTime;
}

app3D::Device &App3D::getDevice()
{
    if(!m_device)
        throw Exception{"Device is nullptr."};

    return *m_device;
}

DefDatabase &App3D::getDefDatabase()
{
    if(!m_defDatabase)
        throw Exception{"Def database is nullptr."};

    return *m_defDatabase;
}

App3D::~App3D()
{
    clean();
}

void App3D::onInit(const app3D::Settings &settings)
{
}

bool App3D::onUpdate()
{
    return false;
}

void App3D::init(int argc, char *argv[])
{
    TRACK;

    if(qApp) {
        throw Exception{"Qt application already exists while initializing engine. "
                        "Engine must be owner of Qt application."};
    }

    LogManager::create(getAppInfo());

    E_INFO("Start of engine initialization.");
    E_INFO("Initializing Qt application.");
    m_qApp = std::make_unique <QApplication> (argc, argv);

    app3D::Settings settings;

    try {
        settings.load();
    }
    catch(const std::exception &e) {
        throw Exception{"Could not load settings.", e};
    }

    m_defDatabase = std::make_shared <DefDatabase> ();

    m_device = app3D::Device::create(settings, m_defDatabase);

    E_RASSERT(m_device, "Device is nullptr.");

    E_INFO("End of engine initialization.");

    try {
        onInit(settings);
    }
    catch(const std::exception &e) {
        throw Exception{"Could not initialize app.", e};
    }
}

void App3D::loop()
{
    TRACK;

    while(true) {
        try {
            Trace::profilerLap();

            m_appTime.update();

            auto &device = getDevice();

            if(!device.update(m_appTime))
                break;

            if(!onUpdate())
                break;

            device.draw();
        }
        catch(const std::exception &e) {
            // we're catching all exceptions here; it's better
            // to continue executing app instead of terminating it
            // with an exception

            E_ERROR("Exception caught in main App3D loop (continuing execution): %s", e.what());
        }
    }

    E_INFO("Application exited main App3D loop normally.");
}

void App3D::clean()
{
    TRACK;

    m_qApp.reset();
}

} // namespace engine
