#ifndef ENGINE_APP_3D_DEVICE_HPP
#define ENGINE_APP_3D_DEVICE_HPP

#include "../util/Trace.hpp"
#include "../util/VideoMode.hpp"
#include "../util/Vec2.hpp"
#include "../util/Color.hpp"
#include "Settings.hpp"

#include <irrlicht/irrlicht.h>

#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace engine { class DefDatabase; class AppTime; }
namespace engine { namespace GUI { class GUIManager; } }
namespace engine { namespace app3D { namespace detail { class GUIRenderer; } } }

namespace engine
{
namespace app3D
{

class EventManager;
class ResourcesManager;
class SceneManager;
class ShadersManager;
class PhysicsManager;
class ParticlesManager;
class CursorManager;

class Device : public Tracked <Device>
{
public:
    Device(const Device &) = delete;

    Device &operator = (const Device &) = delete;

    // the only way to create Device instance is to use this method
    static std::shared_ptr <Device> create(const Settings &settings, const std::shared_ptr <DefDatabase> &defDatabase);

    // called each frame
    bool update(const AppTime &appTime);
    void draw();

    // misc
    void setFog(const Color &color = k_defaultFogColor, float minDist = k_defaultFogMinDist, float maxDist = k_defaultFogMaxDist);
    void setFPPCameraControl(bool controlFPPCamera = true);
    void setWidgetsGetInput(bool getInput);
    bool isUsingDeferredRendering() const;
    void setOnDraw3DCallback(std::function <void()> callback);
    void setOnDraw2DBeforeWidgetsCallback(std::function <void()> callback);
    void setOnDraw2DAfterWidgetsCallback(std::function <void()> callback);
    void setNewVideoParams(int width, int height, bool fullscreen, int antialiasing, bool vsync);
    IntVec2 getScreenSize();

    const Color &getFogColor() const;
    std::shared_ptr <Device> getPtr();
    int getFPS();
    irr::IrrlichtDevice &getIrrDevice() const;
    const Settings::Video3D &getVideoSettings() const;
    SceneManager &getSceneManager();
    ResourcesManager &getResourcesManager();
    EventManager &getEventManager();
    GUI::GUIManager &getGUIManager();
    ShadersManager &getShadersManager();
    PhysicsManager &getPhysicsManager();
    ParticlesManager &getParticlesManager();
    CursorManager &getCursorManager();
    DefDatabase &getDefDatabase() const;

    ~Device();

private:
    Device();

    void init(const Settings &settings);
    bool isVideoModeAvailable(int width, int height);
    void createIrrlichtDevice(int width, int height, bool fullscreen, int antialiasing, bool vsync);
    void dropIrrObjects();
    void updateAudioListener();

    detail::GUIRenderer &getGUIRenderer();

    static const Color k_defaultFogColor;
    static const float k_defaultFogMinDist;
    static const float k_defaultFogMaxDist;

    std::weak_ptr <Device> m_ptr;
    irr::IrrlichtDevice *m_irrDevice;
    std::shared_ptr <DefDatabase> m_defDatabase;
    std::shared_ptr <detail::GUIRenderer> m_GUIRenderer;
    std::unique_ptr <EventManager> m_eventManager;
    std::shared_ptr <GUI::GUIManager> m_GUIManager;
    std::unique_ptr <ShadersManager> m_shadersManager;
    std::unique_ptr <ResourcesManager> m_resourcesManager;
    std::unique_ptr <SceneManager> m_sceneManager;
    std::unique_ptr <PhysicsManager> m_physicsManager;
    std::unique_ptr <ParticlesManager> m_particlesManager;
    std::unique_ptr <CursorManager> m_cursorManager;
    std::vector <VideoMode> m_videoModes;
    Settings::Video3D m_videoSettings;

    Color m_fogColor;
    float m_fogMinDist;
    float m_fogMaxDist;

    std::function <void()> m_onDraw3D;
    std::function <void()> m_onDraw2DBeforeWidgets;
    std::function <void()> m_onDraw2DAfterWidgets;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_DEVICE_HPP
