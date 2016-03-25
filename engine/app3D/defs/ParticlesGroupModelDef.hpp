#ifndef ENGINE_APP_3D_PARTICLES_GROUP_MODEL_DEF_HPP
#define ENGINE_APP_3D_PARTICLES_GROUP_MODEL_DEF_HPP

#include "../../util/Enum.hpp"
#include "../../util/Range.hpp"
#include "ResourceDef.hpp"

#include <Core/SPK_DEF.h>

namespace SPK { class Model; }

namespace engine
{
namespace app3D
{

class ParticlesGroupModelDef : public ResourceDef, public Tracked <ParticlesGroupModelDef>
{
public:
    class Param : public DataFile::Saveable
    {
    public:
        ENUM_DECL(Type,
            None,
            Constant,
            StartEnd,
            RandomStartRandomEnd);

        Param();

        void expose(DataFile::Node &node) override;

        void multiplyBy(float factor);
        void setParamForModel(SPK::Model &model, SPK::ModelParam param);
        const Type &getType() const;

    private:
        Type m_type;
        float m_constant;
        FloatRange m_startEnd;
        FloatRange m_randomStart;
        FloatRange m_randomEnd;
    };

    class Interpolator : public DataFile::Saveable
    {
    public:
        struct Entry : public DataFile::Saveable
        {
            void expose(DataFile::Node &node) override;

            float timePercentage{};
            float multiplier{};
            float multiplierB{};
        };

        void expose(DataFile::Node &node) override;

        void multiplyBy(float factor);
        bool hasAnyEntry() const;
        const std::vector <Entry> &getEntries() const;

    private:
        std::vector <Entry> m_entries;
    };

    ParticlesGroupModelDef();

    void expose(DataFile::Node &node) override;
    void onLoadedAllDefs(DefDatabase &defDatabase) override;
    void dropIrrObjects() override;
    void reloadIrrObjects() override;

    SPK::Model &getSPKModel();

private:
    using base = ResourceDef;

    struct InheritScaledInfo : public DataFile::Saveable
    {
        void expose(DataFile::Node &node) override;

        bool isLoaded{};
        std::string def_defName;
        float scale{};
    };

    void createSPKModel();
    void copyMembersScaled(ParticlesGroupModelDef &copyTo, float scale) const;

    InheritScaledInfo m_inheritScaledInfo;
    Param m_redParam;
    Param m_greenParam;
    Param m_blueParam;
    Param m_alphaParam;
    Param m_angleParam;
    Param m_sizeParam;
    IntRange m_randomTextureIndex;
    FloatRange m_lifeTime;
    Interpolator m_sizeInterpolator;
    Interpolator m_alphaInterpolator;
    SPK::Model *m_SPKModel;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_PARTICLES_GROUP_MODEL_DEF_HPP
