#ifndef ENGINE_APP_3D_HPP
#define ENGINE_APP_3D_HPP

#include "util/Trace.hpp"
#include "util/AppTime.hpp"

#include <string>
#include <memory>

class QApplication;

namespace engine { namespace app3D { class Device; class Settings; } }

namespace engine
{

class DefDatabase;

class App3D : public Tracked <App3D>
{
public:
    App3D();
    App3D(const App3D &) = delete;

    App3D &operator = (const App3D &) = delete;

    virtual const AppInfo &getAppInfo() const = 0;

    void exec(int argc, char *argv[]);
    AppTime &getAppTime();
    app3D::Device &getDevice();
    DefDatabase &getDefDatabase();

    virtual ~App3D();

protected:
    virtual void onInit(const app3D::Settings &settings);
    virtual bool onUpdate();

private:
    void init(int argc, char *argv[]);
    void loop();
    void clean();

    std::unique_ptr <QApplication> m_qApp;
    AppTime m_appTime;
    std::shared_ptr <DefDatabase> m_defDatabase;
    std::shared_ptr <app3D::Device> m_device;
};

} // namespace engine

#endif // ENGINE_APP_HPP
