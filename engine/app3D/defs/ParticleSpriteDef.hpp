#ifndef ENGINE_APP_3D_PARTICLE_SPRITE_DEF_HPP
#define ENGINE_APP_3D_PARTICLE_SPRITE_DEF_HPP

#include "../../util/Enum.hpp"
#include "../../util/Vec2.hpp"
#include "../../util/Vec3.hpp"
#include "ResourceDef.hpp"

namespace SPK { namespace IRR { class IRRQuadRenderer; } }

namespace engine
{
namespace app3D
{

class ParticleSpriteDef : public ResourceDef, public Tracked <ParticleSpriteDef>
{
public:
    ENUM_DECL(Blending,
        Alpha,
        Add);

    ENUM_DECL(Orientation,
        Normal,
        DirectionAligned,
        Fixed);

    ParticleSpriteDef();

    void expose(DataFile::Node &node) override;
    void onLoadedAllDefs(DefDatabase &defDatabase) override;
    void dropIrrObjects() override;
    void reloadIrrObjects() override;

    SPK::IRR::IRRQuadRenderer &getSPKIrrQuadRenderer() const;

private:
    using base = ResourceDef;

    struct InheritScaledInfo : public DataFile::Saveable
    {
        void expose(DataFile::Node &node) override;

        bool isLoaded{};
        std::string def_defName;
        float scale{};
    };

    void createSPKIrrQuadRenderer();
    void copyMembersScaled(ParticleSpriteDef &copyTo, float scale) const;

    InheritScaledInfo m_inheritScaledInfo;
    std::string m_texturePath;
    FloatVec2 m_scale;
    Blending m_blending;
    Orientation m_orientation;
    IntVec2 m_texturesCountInAtlas;
    bool m_useAlphaTest;
    FloatVec3 m_lookVector;
    FloatVec3 m_upVector;
    SPK::IRR::IRRQuadRenderer *m_SPKIrrQuadRenderer;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_PARTICLE_SPRITE_DEF_HPP
