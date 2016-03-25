#ifndef APP_CORE_HPP
#define APP_CORE_HPP

#include "engine/util/Trace.hpp"
#include "engine/App3D.hpp"
#include "engine/AppInfo.hpp"

namespace app
{

class EventReceiver;
class World;
class ThisPlayer;
class MainGUI;
class DefsCache;
class SoundPool;
class EffectsPool;

class Core : public engine::App3D, public engine::Tracked <Core>
{
public:
    Core();

    const engine::AppInfo &getAppInfo() const override;

    World &getWorld();
    ThisPlayer &getThisPlayer();
    MainGUI &getMainGUI();
    DefsCache &getDefsCache();
    SoundPool &getSoundPool();
    EffectsPool &getEffectsPool();

    ~Core() override;

protected:
    void onInit(const engine::app3D::Settings &settings) override;
    bool onUpdate() override;

private:
    void updateCameraPos();
    void loadAllDefs(const engine::app3D::Settings &settings);
    void loadDefs(const std::string &modPath);

    engine::AppInfo m_appInfo;
    std::shared_ptr <EventReceiver> m_eventReceiver;
    std::unique_ptr <World> m_world;
    std::unique_ptr <ThisPlayer> m_thisPlayer;
    std::unique_ptr <MainGUI> m_mainGUI;
    std::unique_ptr <DefsCache> m_defsCache;
    std::unique_ptr <SoundPool> m_soundPool;
    std::unique_ptr <EffectsPool> m_effectsPool;
};

} // namespace app

#endif // APP_CORE_HPP
