#include "Device.hpp"

#include "../util/DefDatabase.hpp"
#include "../util/AppTime.hpp"
#include "../util/Exception.hpp"
#include "../util/LogManager.hpp"
#include "../GUI/GUIManager.hpp"
#include "managers/EventManager.hpp"
#include "managers/PhysicsManager.hpp"
#include "managers/ShadersManager.hpp"
#include "managers/SceneManager.hpp"
#include "managers/ResourcesManager.hpp"
#include "managers/ResourcesManager.hpp"
#include "managers/ParticlesManager.hpp"
#include "managers/CursorManager.hpp"
#include "detail/GUIRenderer.hpp"
#include "IrrlichtConversions.hpp"

#include <SFML/Audio.hpp>

#include <fstream>
#include <string>

namespace engine
{
namespace app3D
{

/*
 * TODO: make sure that dropIrrObjects() and reloadIrrObjects() are called for all
 * Irrlicht resources holders, and that they all work.
 */

std::shared_ptr <Device> Device::create(const Settings &settings, const std::shared_ptr <DefDatabase> &defDatabase)
{
    TRACK;

    if(!defDatabase)
        throw Exception("Def database is nullptr.");

    // Device constructor is private
    std::shared_ptr <Device> ptr(new Device{});

    ptr->m_ptr = ptr;
    ptr->m_defDatabase = defDatabase;
    ptr->init(settings);

    return ptr;
}

bool Device::update(const AppTime &appTime)
{
    TRACK;

    if(!getIrrDevice().run())
        return false;

    getShadersManager().clearPointLights(); // Lights update method will re-add point lights

    getPhysicsManager().update(appTime);
    getSceneManager().update(appTime);
    getGUIRenderer().update();
    getGUIManager().update(appTime);
    getParticlesManager().update();
    updateAudioListener();

    return true;
}

void Device::draw()
{
    TRACK;

    auto &irrDevice = getIrrDevice();
    auto &videoDriver = *irrDevice.getVideoDriver();
    auto &sceneManager = getSceneManager();

    videoDriver.beginScene(true, true, irr::video::SColor{255, 0, 0, 0});

    irrDevice.getSceneManager()->drawAll();

    if(m_onDraw3D)
        m_onDraw3D();

    videoDriver.clearZBuffer();

    sceneManager.drawFPPModels();

    sceneManager.drawHighlightedNode();

    if(m_onDraw2DBeforeWidgets)
        m_onDraw2DBeforeWidgets();

    getGUIManager().draw();

    if(m_onDraw2DAfterWidgets)
        m_onDraw2DAfterWidgets();

    videoDriver.endScene();
}

void Device::setFog(const Color &color, float minDist, float maxDist)
{
    m_fogColor = color;
    m_fogMinDist = minDist;
    m_fogMaxDist = maxDist;

    const auto &irrColor = IrrlichtConversions::toColor(m_fogColor);

    getIrrDevice().getVideoDriver()->setFog(irrColor, irr::video::EFT_FOG_LINEAR, m_fogMinDist, m_fogMaxDist, 0.f, false, false);

    // sky color has always the same color as fog so they blend nicely

    getShadersManager().setSkyColor(color);
}

void Device::setFPPCameraControl(bool controlFPPCamera)
{
    TRACK;

    auto &cursorManager = getCursorManager();
    auto &irrCamera = getSceneManager().getIrrCamera();

    if(controlFPPCamera) {
        cursorManager.showCursor(false);
        irrCamera.setInputReceiverEnabled(true);
    }
    else {
        cursorManager.showCursor(true);
        irrCamera.setInputReceiverEnabled(false);
    }
}

void Device::setWidgetsGetInput(bool getInput)
{
    auto &eventManager = getEventManager();

    if(getInput)
        eventManager.registerEventReceiverAtTheBeginning(m_GUIManager);
    else {
        E_DASSERT(m_GUIManager, "GUI manager is nullptr.");
        eventManager.deregisterEventReceiver(*m_GUIManager);
    }
}

bool Device::isUsingDeferredRendering() const
{
    // TODO: add deferred rendering support

    return false;
}

void Device::setOnDraw3DCallback(std::function <void()> callback)
{
    m_onDraw3D = callback;
}

void Device::setOnDraw2DBeforeWidgetsCallback(std::function <void()> callback)
{
    m_onDraw2DBeforeWidgets = callback;
}

void Device::setOnDraw2DAfterWidgetsCallback(std::function <void()> callback)
{
    m_onDraw2DAfterWidgets = callback;
}

void Device::setNewVideoParams(int width, int height, bool fullscreen, int antialiasing, bool vsync)
{
    TRACK;

    if(width != m_videoSettings.width || height != m_videoSettings.height ||
       fullscreen != m_videoSettings.fullscreen || antialiasing != m_videoSettings.antialiasing ||
       vsync != m_videoSettings.vsync) {
        E_INFO("Setting new video params. Recreating Irrlicht device.");
        createIrrlichtDevice(width, height, fullscreen, antialiasing, vsync);
    }
}

IntVec2 Device::getScreenSize()
{
    const auto &size = getIrrDevice().getVideoDriver()->getScreenSize();

    return {static_cast <int> (size.Width),
            static_cast <int> (size.Height)};
}

const Color &Device::getFogColor() const
{
    return m_fogColor;
}

std::shared_ptr <Device> Device::getPtr()
{
    const auto &shared = m_ptr.lock();

    if(!shared)
        throw Exception{"Device could not lock its own weak pointer."};

    return shared;
}

int Device::getFPS()
{
    return getIrrDevice().getVideoDriver()->getFPS();
}

irr::IrrlichtDevice &Device::getIrrDevice() const
{
    if(!m_irrDevice)
        throw Exception{"Irrlicht device is nullptr."};

    return *m_irrDevice;
}

const Settings::Video3D &Device::getVideoSettings() const
{
    return m_videoSettings;
}

ResourcesManager &Device::getResourcesManager()
{
    if(!m_resourcesManager)
        throw Exception{"Resources manager is nullptr."};

    return *m_resourcesManager;
}

SceneManager &Device::getSceneManager()
{
    if(!m_sceneManager)
        throw Exception{"Scene manager is nullptr."};

    return *m_sceneManager;
}

EventManager &Device::getEventManager()
{
    if(!m_eventManager)
        throw Exception{"Event manager is nullptr."};

    return *m_eventManager;
}

GUI::GUIManager &Device::getGUIManager()
{
    if(!m_GUIManager)
        throw Exception{"GUI manager is nullptr."};

    return *m_GUIManager;
}

ShadersManager &Device::getShadersManager()
{
    if(!m_shadersManager)
        throw Exception{"Shaders manager is nullptr."};

    return *m_shadersManager;
}

PhysicsManager &Device::getPhysicsManager()
{
    if(!m_physicsManager)
        throw Exception{"Physics manager is nullptr."};

    return *m_physicsManager;
}

ParticlesManager &Device::getParticlesManager()
{
    if(!m_particlesManager)
        throw Exception{"Particles manager is nullptr."};

    return *m_particlesManager;
}

CursorManager &Device::getCursorManager()
{
    if(!m_cursorManager)
        throw Exception{"Cursor manager is nullptr."};

    return *m_cursorManager;
}

DefDatabase &Device::getDefDatabase() const
{
    E_DASSERT(m_defDatabase, "Def database is nullptr.");
    return *m_defDatabase;
}

Device::~Device()
{
    dropIrrObjects();

    if(m_irrDevice)
        m_irrDevice->drop();
}

Device::Device()
    : m_irrDevice{},
      m_fogColor{k_defaultFogColor},
      m_fogMinDist{k_defaultFogMinDist},
      m_fogMaxDist{k_defaultFogMaxDist}
{
}

void Device::init(const Settings &settings)
{
    TRACK;

    E_INFO("Initializing Device.");

    if(m_ptr.expired()) {
        throw Exception{"Pointer to self (Device) is nullptr. "
                              "This class must be able to share itself."};
    }

    if(!m_defDatabase)
        throw Exception{"Def database is nullptr."};

    m_videoSettings = settings.video3D;

    if(!irr::IrrlichtDevice::isDriverSupported(irr::video::EDT_OPENGL))
        throw Exception{"OpenGL driver is not supported."};

    EventManager tempEventManager;

    irr::SIrrlichtCreationParameters creationParams;
    creationParams.DriverType = irr::video::EDT_NULL;
    creationParams.EventReceiver = &tempEventManager;

    E_INFO("Creating temporary Irrlicht device with nullptr driver.");

    if(!(m_irrDevice = irr::createDeviceEx(creationParams)))
        throw Exception{"Could not create temporary Irrlicht device with nullptr driver."};

    auto *videoModes = m_irrDevice->getVideoModeList();

    if(!videoModes || videoModes->getVideoModeCount() <= 0)
        throw Exception{"No video mode available."};

    for(irr::s32 i = 0; i < videoModes->getVideoModeCount(); ++i) {
        if(videoModes->getVideoModeDepth(i) == 32) {
            m_videoModes.push_back({static_cast <int> (videoModes->getVideoModeResolution(i).Width),
                                    static_cast <int> (videoModes->getVideoModeResolution(i).Height)});
        }
    }

    if(m_videoModes.empty())
        throw Exception{"No 32 bit video mode available."};

    E_INFO("Removing temporary Irrlicht device.");

    m_irrDevice->drop();
    m_irrDevice = nullptr;

    m_eventManager = std::make_unique <EventManager> ();

    createIrrlichtDevice(settings.video3D.width, settings.video3D.height,
                         settings.video3D.fullscreen, settings.video3D.antialiasing,
                         settings.video3D.vsync);

    E_RASSERT(m_irrDevice, "Irrlicht device is nullptr.");

    const auto &driver = *m_irrDevice->getVideoDriver();

    if(!driver.queryFeature(irr::video::EVDF_MIP_MAP))
        throw Exception{"Your graphics card does not support mip maps."};

    if(!driver.queryFeature(irr::video::EVDF_VERTEX_SHADER_1_1))
        throw Exception{"Your graphics card does not support vertex shader 1.1 or higher."};

    if(!driver.queryFeature(irr::video::EVDF_PIXEL_SHADER_1_1))
        throw Exception{"Your graphics card does not support fragment shader 1.1 or higher."};

    if(!driver.queryFeature(irr::video::EVDF_TEXTURE_NSQUARE))
        throw Exception{"Your graphics card does not support non-square textures."};

    if(!driver.queryFeature(irr::video::EVDF_TEXTURE_NPOT))
        throw Exception{"Your graphics card does not support non-power-of-two textures."};

    if(!driver.queryFeature(irr::video::EVDF_MULTITEXTURE))
        throw Exception{"Your graphics card does not support multitexture."};

    if(!driver.queryFeature(irr::video::EVDF_RENDER_TO_TARGET))
        E_INFO("Render to target driver feature not supported.");

    if(!driver.queryFeature(irr::video::EVDF_FRAMEBUFFER_OBJECT))
        E_INFO("Framebuffer object driver feature not supported.");

    if(!driver.queryFeature(irr::video::EVDF_MULTIPLE_RENDER_TARGETS))
        E_INFO("Multiple render targets driver feature not supported.");

    /* TODO:
     * if(isDeferredRenderingAvailable())
     *     E_INFO("Deferred rendering available.");
     * else not supported
     */

    m_resourcesManager = std::make_unique <ResourcesManager> (*this, settings);
    m_GUIRenderer = std::make_shared <detail::GUIRenderer> (*this);
    m_GUIManager = std::make_shared <GUI::GUIManager> (m_GUIRenderer);
    m_shadersManager = std::make_unique <ShadersManager> (*this, settings);
    m_sceneManager = std::make_unique <SceneManager> (*this);
    m_physicsManager = std::make_unique <PhysicsManager> (*this);
    m_particlesManager = std::make_unique <ParticlesManager> (*this);
    m_cursorManager = std::make_unique <CursorManager> (*this);
}

bool Device::isVideoModeAvailable(int width, int height)
{
    return std::any_of(m_videoModes.begin(), m_videoModes.end(), [&](const auto &mode) {
        return mode.getWidth() == width && mode.getHeight() == height;
    });
}

detail::GUIRenderer &Device::getGUIRenderer()
{
    E_DASSERT(m_GUIRenderer, "GUI renderer is nullptr.");

    return *m_GUIRenderer;
}

const Color Device::k_defaultFogColor{255, 255, 255};
const float Device::k_defaultFogMinDist{180.f};
const float Device::k_defaultFogMaxDist{500.f};

void Device::createIrrlichtDevice(int width, int height, bool fullscreen, int antialiasing, bool vsync)
{
    TRACK;

    E_INFO("Creating Irrlicht device (%dx%d, %s, antialiasing %d, %s).",
           width,
           height,
           fullscreen ? "fullscreen" : "windowed",
           antialiasing,
           vsync ? "vsync" : "no vsync");

    if(m_irrDevice) {
        E_INFO("Removing previous Irrlicht device and cleaning up.");

        dropIrrObjects();

        E_INFO("Previous Irrlicht device removed.");
    }

    if(m_videoModes.empty())
        throw Exception{"No video mode available."};

    if(!isVideoModeAvailable(width, height)) {
        E_WARNING("Video mode %dx%d not available. Using first one available (%dx%d).",
                  width,
                  height,
                  m_videoModes[0].getWidth(),
                  m_videoModes[0].getHeight());

        width = m_videoModes[0].getWidth();
        height = m_videoModes[0].getHeight();
    }

    if(antialiasing < 0) {
        E_WARNING("Invalid antialiasing level (%d). Using 0 antialiasing level.", antialiasing);
        antialiasing = 0;
    }

    irr::SIrrlichtCreationParameters creationParams;
    creationParams.DriverType = irr::video::EDT_OPENGL;
    creationParams.WindowSize = {static_cast <irr::u32> (width), static_cast <irr::u32> (height)};
    creationParams.Fullscreen = fullscreen;
    creationParams.AntiAlias = antialiasing;
    creationParams.Bits = 32;
    creationParams.Vsync = vsync;
    creationParams.EventReceiver = &getEventManager();

    if(!(m_irrDevice = irr::createDeviceEx(creationParams)))
        throw Exception{"Could not create Irrlicht device."};

    // TODO: engine user should somehow specify app title
    m_irrDevice->setWindowCaption(L"App");

    auto &videoDriver = *m_irrDevice->getVideoDriver();

    videoDriver.setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, true);
    videoDriver.setTextureCreationFlag(irr::video::ETCF_ALWAYS_32_BIT, true);

    const auto &irrFogColor = IrrlichtConversions::toColor(m_fogColor);
    videoDriver.setFog(irrFogColor, irr::video::EFT_FOG_LINEAR, m_fogMinDist, m_fogMaxDist, 0.f, false, false);

    m_videoSettings.width = width;
    m_videoSettings.height = height;
    m_videoSettings.fullscreen = fullscreen;
    m_videoSettings.antialiasing = antialiasing;
    m_videoSettings.vsync = vsync;

    auto &material2D = videoDriver.getMaterial2D();

    material2D.TextureLayer[0].BilinearFilter = false;
    material2D.TextureLayer[0].TrilinearFilter = true;
    material2D.AntiAliasing = irr::video::EAAM_FULL_BASIC;
    material2D.UseMipMaps = true; // TODO: experimental (actually gives good results for scaled images)

    auto &sceneManager = *m_irrDevice->getSceneManager();
    sceneManager.setAmbientLight({1.f, 1.f, 1.f});

    E_INFO("Irrlicht device created.");

    if(m_resourcesManager)
        m_resourcesManager->reloadIrrObjects();

    if(m_sceneManager)
        m_sceneManager->reloadIrrObjects();

    if(m_cursorManager)
        m_cursorManager->reloadIrrObjects();
}

void Device::dropIrrObjects()
{
    if(m_cursorManager)
        m_cursorManager->dropIrrObjects();

    if(m_sceneManager)
        m_sceneManager->dropIrrObjects();

    if(m_resourcesManager)
        m_resourcesManager->dropIrrObjects();

    if(m_irrDevice) {
        m_irrDevice->drop();
        m_irrDevice = nullptr;
    }
}

void Device::updateAudioListener()
{
    const auto &sceneManager = getSceneManager();

    const auto &cameraPos = sceneManager.getCameraPosition();
    const auto &cameraLookVec = sceneManager.getCameraLookVec();

    sf::Listener::setPosition(cameraPos.x, cameraPos.y, cameraPos.z);
    sf::Listener::setDirection(cameraLookVec.x, cameraLookVec.y, cameraLookVec.z);
}

} // namespace app3D
} // namespace engine
