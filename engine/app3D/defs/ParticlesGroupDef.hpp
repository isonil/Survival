#ifndef ENGINE_APP_3D_PARTICLES_GROUP_DEF_HPP
#define ENGINE_APP_3D_PARTICLES_GROUP_DEF_HPP

#include "../../util/Enum.hpp"
#include "../../util/Vec3.hpp"
#include "ResourceDef.hpp"

namespace engine
{
namespace app3D
{

class ParticlesGroupModelDef;
class ParticleSpriteDef;

class ParticlesGroupDef : public ResourceDef, public Tracked <ParticlesGroupDef>
{
public:
    class Emitter : public DataFile::Saveable
    {
    public:
        struct ZoneSphere : public DataFile::Saveable
        {
            void expose(DataFile::Node &node) override;

            FloatVec3 position;
            float radius{};
        };

        ENUM_DECL(Type,
            Normal,
            Random,
            Static,
            Spheric,
            Straight
        );

        Emitter();

        void expose(DataFile::Node &node) override;

        void multiplySizeBy(float factor);
        const Type &getType() const;
        const FloatVec3 &getDirection() const;
        const FloatRange &getAngle() const;
        const ZoneSphere &getZoneSphere() const;
        int getFlow() const;
        int getTank() const;
        bool getFullZone() const;
        const FloatRange &getForce() const;

    private:
        Type m_type;
        FloatVec3 m_direction;
        FloatRange m_angle;
        ZoneSphere m_zoneSphere;
        bool m_fullZone;
        int m_flow;
        int m_tank;
        FloatRange m_force;
    };

    ParticlesGroupDef();

    void expose(DataFile::Node &node) override;
    void onLoadedAllDefs(DefDatabase &defDatabase) override;
    void dropIrrObjects() override;
    void reloadIrrObjects() override;

    ParticlesGroupModelDef &getParticlesGroupModelDef() const;
    const std::shared_ptr <ParticlesGroupModelDef> &getParticlesGroupModelDefPtr() const;

    ParticleSpriteDef &getParticleSpriteDef() const;
    const std::shared_ptr <ParticleSpriteDef> &getParticleSpriteDefPtr() const;

    int getMaxParticlesCount() const;
    const FloatVec3 &getGravity() const;
    float getFriction() const;
    const std::vector <Emitter> &getEmitters() const;

private:
    using base = ResourceDef;

    struct InheritScaledInfo : public DataFile::Saveable
    {
        void expose(DataFile::Node &node) override;

        bool isLoaded{};
        std::string def_defName;
        float scale{};
    };

    void copyMembersScaled(ParticlesGroupDef &copyTo, float scale) const;

    InheritScaledInfo m_inheritScaledInfo;

    std::string m_particlesGroupModelDef_defName;
    std::shared_ptr <ParticlesGroupModelDef> m_particlesGroupModelDef;

    std::string m_particleSpriteDef_defName;
    std::shared_ptr <ParticleSpriteDef> m_particleSpriteDef;

    int m_maxParticlesCount;
    FloatVec3 m_gravity;
    float m_friction;

    std::vector <Emitter> m_emitters;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_PARTICLES_GROUP_DEF_HPP

