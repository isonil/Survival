#include "Settings.hpp"

#include "../util/LogManager.hpp"
#include "../util/Trace.hpp"

namespace engine
{
namespace app3D
{

void Settings::Video3D::expose(DataFile::Node &node)
{
    node.var(width, "width");
    node.var(height, "height");
    node.var(fullscreen, "fullscreen");
    node.var(antialiasing, "antialiasing");
    node.var(vsync, "vsync");
    node.var(FOV, "FOV");
    node.var(deferredRendering, "deferredRendering");
    node.var(SSAO, "SSAO");
    node.var(lightShaftsEffect, "lightShaftsEffect");
}

void Settings::Audio::expose(DataFile::Node &node)
{
    node.var(mute, "mute");
    node.var(volume, "volume");
}

void Settings::Mods::Mod::expose(DataFile::Node &node)
{
    node.var(path, "path");
    node.var(enabled, "enabled");
}

void Settings::Mods::expose(DataFile::Node &node)
{
    node.var(mods, "mods");
}

void Settings::expose(DataFile::Node &node)
{
    node.var(engineVersion, "engineVersion");
    node.var(video3D, "video3D");
    node.var(audio, "audio");
    node.var(mods, "mods");
    node.var(appVersion, "appVersion");
}

void Settings::load()
{
    TRACK;

    E_INFO("Loading settings file \"%s\".", k_settingsPath.c_str());

    DataFile data;

    data.open(k_settingsPath, true);

    data.load(*this, "settings");
}

const std::string Settings::k_settingsPath{"settings.yaml"};

} // namespace app3D
} // namespace engine
