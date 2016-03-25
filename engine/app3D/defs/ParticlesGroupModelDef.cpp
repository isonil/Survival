#include "ParticlesGroupModelDef.hpp"

#include "../../util/DefDatabase.hpp"

#undef emit // qt keyword
#include <SPK.h>

namespace engine
{
namespace app3D
{

ENUM_DEF(ParticlesGroupModelDef::Param::Type, Type,
    None,
    Constant,
    StartEnd,
    RandomStartRandomEnd);

ParticlesGroupModelDef::Param::Param()
    : m_type{Type::None},
      m_constant{1.f},
      m_startEnd{1.f, 1.f},
      m_randomStart{1.f, 1.f},
      m_randomEnd{1.f, 1.f}
{
}

void ParticlesGroupModelDef::Param::expose(DataFile::Node &node)
{
    node.var(m_type, "type");
    node.var(m_constant, "constant", 1.f);
    node.var(m_startEnd, "startEnd", {1.f, 1.f});
    node.var(m_randomStart, "randomStart", {1.f, 1.f});
    node.var(m_randomEnd, "randomEnd", {1.f, 1.f});
}

void ParticlesGroupModelDef::Param::multiplyBy(float factor)
{
    if(factor < 0.f)
        factor = 0.f;

    m_constant *= factor;
    m_startEnd.from *= factor;
    m_startEnd.to *= factor;
    m_randomStart.from *= factor;
    m_randomStart.to *= factor;
    m_randomEnd.from *= factor;
    m_randomEnd.to *= factor;
}

void ParticlesGroupModelDef::Param::setParamForModel(SPK::Model &model, SPK::ModelParam param)
{
    if(m_type == Type::Constant)
        model.setParam(param, m_constant);
    else if(m_type == Type::StartEnd)
        model.setParam(param, m_startEnd.from, m_startEnd.to);
    else if(m_type == Type::RandomStartRandomEnd)
        model.setParam(param, m_randomStart.from, m_randomStart.to, m_randomEnd.from, m_randomEnd.to);
}

const ParticlesGroupModelDef::Param::Type &ParticlesGroupModelDef::Param::getType() const
{
    return m_type;
}

void ParticlesGroupModelDef::Interpolator::Entry::expose(DataFile::Node &node)
{
    node.var(timePercentage, "timePercentage");
    node.var(multiplier, "multiplier");
    node.var(multiplierB, "multiplierB", multiplier);

    if(node.getActivityType() == DataFile::Activity::Type::Loading) {
        if(timePercentage < 0.f || timePercentage > 1.f)
            throw Exception{"Time percentage must be between 0.0 and 1.0."};
    }
}

void ParticlesGroupModelDef::Interpolator::expose(DataFile::Node &node)
{
    node.var(m_entries, "entries");

    if(node.getActivityType() == DataFile::Activity::Type::Loading) {
        for(size_t i = 1; i < m_entries.size(); ++i) {
            if(m_entries[i].timePercentage < m_entries[i - 1].timePercentage)
                throw Exception{"Time percentage values must be in ascending order."};
        }
    }
}

void ParticlesGroupModelDef::Interpolator::multiplyBy(float factor)
{
    if(factor < 0.f)
        factor = 0.f;

    for(auto &elem : m_entries) {
        elem.multiplier *= factor;
        elem.multiplierB *= factor;
    }
}

bool ParticlesGroupModelDef::Interpolator::hasAnyEntry() const
{
    return !m_entries.empty();
}

const std::vector <ParticlesGroupModelDef::Interpolator::Entry> &ParticlesGroupModelDef::Interpolator::getEntries() const
{
    return m_entries;
}

ParticlesGroupModelDef::ParticlesGroupModelDef()
    : m_randomTextureIndex{0, 1},
      m_SPKModel{}
{
}

void ParticlesGroupModelDef::expose(DataFile::Node &node)
{
    base::expose(node);

    node.var(m_inheritScaledInfo, "inheritScaled", {});
    node.var(m_redParam, "redParam", {});
    node.var(m_greenParam, "greenParam", {});
    node.var(m_blueParam, "blueParam", {});
    node.var(m_alphaParam, "alphaParam", {});
    node.var(m_angleParam, "angleParam", {});
    node.var(m_sizeParam, "sizeParam", {});
    node.var(m_randomTextureIndex, "randomTextureIndex", {0, 1});
    node.var(m_lifeTime, "lifeTime", {1.f, 1.f});
    node.var(m_sizeInterpolator, "sizeInterpolator", {});
    node.var(m_alphaInterpolator, "alphaInterpolator", {});

    if(node.getActivityType() == DataFile::Activity::Type::Loading) {
        if(!m_inheritScaledInfo.isLoaded) {
            if(m_randomTextureIndex.from < 0 || m_randomTextureIndex.to < 0)
                throw Exception{"Random texture index can't be negative."};

            if(m_lifeTime.from < 0.f || m_lifeTime.to < 0.f)
                throw Exception{"Life time can't be negative."};
        }
    }
}

void ParticlesGroupModelDef::onLoadedAllDefs(DefDatabase &defDatabase)
{
    if(m_inheritScaledInfo.isLoaded) {
        const auto &def = *defDatabase.getDef <ParticlesGroupModelDef> (m_inheritScaledInfo.def_defName);

        def.copyMembersScaled(*this, m_inheritScaledInfo.scale);

        if(m_SPKModel)
            delete m_SPKModel;

        m_SPKModel = nullptr;
    }
}

void ParticlesGroupModelDef::dropIrrObjects()
{
    if(m_SPKModel)
        delete m_SPKModel;

    m_SPKModel = nullptr;
}

void ParticlesGroupModelDef::reloadIrrObjects()
{
    createSPKModel();
}

SPK::Model &ParticlesGroupModelDef::getSPKModel()
{
    if(!m_SPKModel)
        createSPKModel();

    E_DASSERT(m_SPKModel, "SPARK model is nullptr.");

    return *m_SPKModel;
}

void ParticlesGroupModelDef::InheritScaledInfo::expose(DataFile::Node &node)
{
    node.var(def_defName, "def");
    node.var(scale, "scale");

    if(node.getActivityType() == DataFile::Activity::Type::Loading) {
        if(scale < 0.f)
            throw Exception{"Scale can't be negative."};

        isLoaded = true;
    }
}

void ParticlesGroupModelDef::createSPKModel()
{
    if(m_SPKModel)
        delete m_SPKModel;

    m_SPKModel = nullptr;

    int enableFlags{};

    if(m_redParam.getType() != Param::Type::None)
        enableFlags |= SPK::FLAG_RED;

    if(m_greenParam.getType() != Param::Type::None)
        enableFlags |= SPK::FLAG_GREEN;

    if(m_blueParam.getType() != Param::Type::None)
        enableFlags |= SPK::FLAG_BLUE;

    if(m_alphaParam.getType() != Param::Type::None || m_alphaInterpolator.hasAnyEntry())
        enableFlags |= SPK::FLAG_ALPHA;

    if(m_angleParam.getType() != Param::Type::None)
        enableFlags |= SPK::FLAG_ANGLE;

    if(m_sizeParam.getType() != Param::Type::None || m_sizeInterpolator.hasAnyEntry())
        enableFlags |= SPK::FLAG_SIZE;

    if(m_randomTextureIndex.from != 0 || m_randomTextureIndex.to != 1)
        enableFlags |= SPK::FLAG_TEXTURE_INDEX;

    int mutableFlags{};

    if(m_redParam.getType() == Param::Type::StartEnd ||
       m_redParam.getType() == Param::Type::RandomStartRandomEnd)
        mutableFlags |= SPK::FLAG_RED;

    if(m_greenParam.getType() == Param::Type::StartEnd ||
       m_greenParam.getType() == Param::Type::RandomStartRandomEnd)
        mutableFlags |= SPK::FLAG_GREEN;

    if(m_blueParam.getType() == Param::Type::StartEnd ||
       m_blueParam.getType() == Param::Type::RandomStartRandomEnd)
        mutableFlags |= SPK::FLAG_BLUE;

    if(m_alphaParam.getType() == Param::Type::StartEnd ||
       m_alphaParam.getType() == Param::Type::RandomStartRandomEnd)
        mutableFlags |= SPK::FLAG_ALPHA;

    if(m_angleParam.getType() == Param::Type::StartEnd ||
       m_angleParam.getType() == Param::Type::RandomStartRandomEnd)
        mutableFlags |= SPK::FLAG_ANGLE;

    if(m_sizeParam.getType() == Param::Type::StartEnd ||
       m_sizeParam.getType() == Param::Type::RandomStartRandomEnd)
        mutableFlags |= SPK::FLAG_SIZE;

    int randomFlags{};

    if(m_randomTextureIndex.from != 0 || m_randomTextureIndex.to != 1)
        randomFlags |= SPK::FLAG_TEXTURE_INDEX;

    if(m_redParam.getType() == Param::Type::RandomStartRandomEnd)
        randomFlags |= SPK::FLAG_RED;

    if(m_greenParam.getType() == Param::Type::RandomStartRandomEnd)
        randomFlags |= SPK::FLAG_GREEN;

    if(m_blueParam.getType() == Param::Type::RandomStartRandomEnd)
        randomFlags |= SPK::FLAG_BLUE;

    if(m_alphaParam.getType() == Param::Type::RandomStartRandomEnd)
        randomFlags |= SPK::FLAG_ALPHA;

    if(m_angleParam.getType() == Param::Type::RandomStartRandomEnd)
        randomFlags |= SPK::FLAG_ANGLE;

    if(m_sizeParam.getType() == Param::Type::RandomStartRandomEnd)
        randomFlags |= SPK::FLAG_SIZE;

    int interpolatedFlags{};

    if(m_sizeInterpolator.hasAnyEntry())
        interpolatedFlags |= SPK::FLAG_SIZE;

    if(m_alphaInterpolator.hasAnyEntry())
        interpolatedFlags |= SPK::FLAG_ALPHA;

    m_SPKModel = SPK::Model::create(
        enableFlags,
        mutableFlags,
        randomFlags,
        interpolatedFlags);

    if(!m_SPKModel)
        throw Exception{"Could not create SPARK model."};

    m_redParam.setParamForModel(*m_SPKModel, SPK::PARAM_RED);
    m_greenParam.setParamForModel(*m_SPKModel, SPK::PARAM_GREEN);
    m_blueParam.setParamForModel(*m_SPKModel, SPK::PARAM_BLUE);
    m_alphaParam.setParamForModel(*m_SPKModel, SPK::PARAM_ALPHA);
    m_angleParam.setParamForModel(*m_SPKModel, SPK::PARAM_ANGLE);
    m_sizeParam.setParamForModel(*m_SPKModel, SPK::PARAM_SIZE);

    if(m_randomTextureIndex.from != 0 || m_randomTextureIndex.to != 1)
        m_SPKModel->setParam(SPK::PARAM_TEXTURE_INDEX, m_randomTextureIndex.from, m_randomTextureIndex.to);

    m_SPKModel->setLifeTime(m_lifeTime.from, m_lifeTime.to);
    m_SPKModel->setShared(true);

    if(m_sizeInterpolator.hasAnyEntry()) {
        auto *interpolator = m_SPKModel->getInterpolator(SPK::PARAM_SIZE);

        E_DASSERT(interpolator, "Interpolator is nullptr.");

        for(const auto &elem : m_sizeInterpolator.getEntries()) {
            interpolator->addEntry(elem.timePercentage, elem.multiplier, elem.multiplierB);
        }
    }

    if(m_alphaInterpolator.hasAnyEntry()) {
        auto *interpolator = m_SPKModel->getInterpolator(SPK::PARAM_ALPHA);

        E_DASSERT(interpolator, "Interpolator is nullptr.");

        for(const auto &elem : m_alphaInterpolator.getEntries()) {
            interpolator->addEntry(elem.timePercentage, elem.multiplier, elem.multiplierB);
        }
    }
}

void ParticlesGroupModelDef::copyMembersScaled(ParticlesGroupModelDef &copyTo, float scale) const
{
    copyTo.m_redParam = m_redParam;
    copyTo.m_greenParam = m_greenParam;
    copyTo.m_blueParam = m_blueParam;
    copyTo.m_alphaParam = m_alphaParam;
    copyTo.m_angleParam = m_angleParam;
    copyTo.m_sizeParam = m_sizeParam;
    copyTo.m_randomTextureIndex = m_randomTextureIndex;
    copyTo.m_lifeTime = m_lifeTime;
    copyTo.m_sizeInterpolator = m_sizeInterpolator;
    copyTo.m_alphaInterpolator = m_alphaInterpolator;

    copyTo.m_sizeParam.multiplyBy(scale);
    copyTo.m_sizeInterpolator.multiplyBy(scale);

    if(copyTo.m_SPKModel)
        delete copyTo.m_SPKModel;

    copyTo.m_SPKModel = nullptr;
}

} // namespace app3D
} // namespace engine
