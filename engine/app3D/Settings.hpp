#ifndef ENGINE_APP_3D_APP_3D_SETTINGS_HPP
#define ENGINE_APP_3D_APP_3D_SETTINGS_HPP

#include "../util/DataFile.hpp"
#include "../util/Version.hpp"

#include <string>
#include <vector>

namespace engine
{
namespace app3D
{

struct Settings : public DataFile::Saveable
{
    struct Video3D : public DataFile::Saveable
    {
        void expose(DataFile::Node &node) override;

        int width{1024};
        int height{768};
        bool fullscreen{};
        int antialiasing{2};
        bool vsync{true};
        int FOV{60};
        bool deferredRendering{true};
        bool SSAO{true};
        bool lightShaftsEffect{true};
    };

    struct Audio : public DataFile::Saveable
    {
        void expose(DataFile::Node &node) override;

        bool mute{};
        float volume{1.f};
    };

    struct Mods : public DataFile::Saveable
    {
        struct Mod : public DataFile::Saveable
        {
            void expose(DataFile::Node &node) override;

            std::string path;
            bool enabled{};
        };

        void expose(DataFile::Node &node) override;

        std::vector <Mod> mods;
    };

    void expose(DataFile::Node &node) override;
    void load();

    static const std::string k_settingsPath;

    Version engineVersion;
    Video3D video3D;
    Audio audio;
    Mods mods;
    Version appVersion;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_APP_3D_SETTINGS_HPP

