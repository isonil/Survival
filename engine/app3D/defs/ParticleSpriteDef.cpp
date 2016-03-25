#include "ParticleSpriteDef.hpp"

#include "../../util/DefDatabase.hpp"
#include "../managers/ResourcesManager.hpp"
#include "../Device.hpp"

#undef emit // qt keyword
#include <SPK.h>
#include <SPK_IRR.h>

namespace engine
{
namespace app3D
{

ENUM_DEF(ParticleSpriteDef::Blending, Blending,
    Alpha,
    Add);

ENUM_DEF(ParticleSpriteDef::Orientation, Orientation,
    Normal,
    DirectionAligned,
    Fixed);

ParticleSpriteDef::ParticleSpriteDef()
    : m_scale{1.f, 1.f},
      m_blending{Blending::Alpha},
      m_orientation{Orientation::Normal},
      m_texturesCountInAtlas{1, 1},
      m_useAlphaTest{},
      m_SPKIrrQuadRenderer{}
{
}

void ParticleSpriteDef::expose(DataFile::Node &node)
{
    base::expose(node);

    node.var(m_inheritScaledInfo, "inheritScaled", {});
    node.var(m_texturePath, "texturePath", {});
    node.var(m_scale, "scale", {1.f, 1.f});
    node.var(m_blending, "blending", Blending::Alpha);
    node.var(m_orientation, "orientation", Orientation::Normal);
    node.var(m_texturesCountInAtlas, "texturesCountInAtlas", {1, 1});
    node.var(m_useAlphaTest, "useAlphaTest", {});
    node.var(m_lookVector, "lookVector", {0.f, 0.f, 1.f});
    node.var(m_upVector, "upVector", {0.f, 1.f, 0.f});

    if(node.getActivityType() == DataFile::Activity::Type::Loading) {
        if(!m_inheritScaledInfo.isLoaded) {
            if(m_texturePath.empty())
                throw Exception{"Texture path can't be empty."};

            if(m_scale.x < 0.f || m_scale.y < 0.f)
                throw Exception{"Scale can't be negative."};

            if(m_texturesCountInAtlas.x <= 0 || m_texturesCountInAtlas.y <= 0)
                throw Exception{"Invalid textures count in atlas."};

            if(!deviceExpired())
                createSPKIrrQuadRenderer();
        }
    }
}

void ParticleSpriteDef::onLoadedAllDefs(DefDatabase &defDatabase)
{
    if(m_inheritScaledInfo.isLoaded) {
        const auto &def = *defDatabase.getDef <ParticleSpriteDef> (m_inheritScaledInfo.def_defName);

        def.copyMembersScaled(*this, m_inheritScaledInfo.scale);

        if(!deviceExpired())
            createSPKIrrQuadRenderer();
    }
}

void ParticleSpriteDef::dropIrrObjects()
{
    if(m_SPKIrrQuadRenderer)
        delete m_SPKIrrQuadRenderer;

    m_SPKIrrQuadRenderer = nullptr;
}

void ParticleSpriteDef::reloadIrrObjects()
{
    createSPKIrrQuadRenderer();
}

SPK::IRR::IRRQuadRenderer &ParticleSpriteDef::getSPKIrrQuadRenderer() const
{
    if(!m_SPKIrrQuadRenderer)
        throw Exception{"SPARK irr quad renderer is nullptr."};

    return *m_SPKIrrQuadRenderer;
}

void ParticleSpriteDef::InheritScaledInfo::expose(DataFile::Node &node)
{
    node.var(def_defName, "def");
    node.var(scale, "scale");

    if(node.getActivityType() == DataFile::Activity::Type::Loading) {
        if(scale < 0.f)
            throw Exception{"Scale can't be negative."};

        isLoaded = true;
    }
}

void ParticleSpriteDef::createSPKIrrQuadRenderer()
{
    if(m_SPKIrrQuadRenderer)
        delete m_SPKIrrQuadRenderer;

    auto &device = getDevice_slow();
    auto &irrDevice = device.getIrrDevice();
    auto &resourcesManager = device.getResourcesManager();

    m_SPKIrrQuadRenderer = SPK::IRR::IRRQuadRenderer::create(&irrDevice);

    if(!m_SPKIrrQuadRenderer)
        throw Exception{"Could not create SPARK irr quad renderer."};

    m_SPKIrrQuadRenderer->setScale(m_scale.x, m_scale.y);
    m_SPKIrrQuadRenderer->setTexture(&resourcesManager.loadIrrTexture(m_texturePath, true));
    m_SPKIrrQuadRenderer->setTexturingMode(SPK::TEXTURE_2D);
    m_SPKIrrQuadRenderer->setBlending(m_blending == Blending::Alpha ? SPK::BLENDING_ALPHA : SPK::BLENDING_ADD);
    m_SPKIrrQuadRenderer->enableRenderingHint(SPK::DEPTH_WRITE, false);
    m_SPKIrrQuadRenderer->setAtlasDimensions(m_texturesCountInAtlas.x, m_texturesCountInAtlas.y);
    m_SPKIrrQuadRenderer->setShared(true);
    m_SPKIrrQuadRenderer->setAlphaTestThreshold(0.f);

    if(m_orientation == Orientation::DirectionAligned)
        m_SPKIrrQuadRenderer->setOrientation(SPK::DIRECTION_ALIGNED);
    else if(m_orientation == Orientation::Fixed) {
        m_SPKIrrQuadRenderer->setOrientation(SPK::FIXED_ORIENTATION);
        m_SPKIrrQuadRenderer->lookVector.set(m_lookVector.x, m_lookVector.y, m_lookVector.z);
        m_SPKIrrQuadRenderer->upVector.set(m_upVector.x, m_upVector.y, m_upVector.z);
    }

    if(m_useAlphaTest)
        m_SPKIrrQuadRenderer->enableRenderingHint(SPK::ALPHA_TEST, true);
}

void ParticleSpriteDef::copyMembersScaled(ParticleSpriteDef &copyTo, float scale) const
{
    copyTo.m_texturePath = m_texturePath;
    copyTo.m_scale = m_scale * scale;
    copyTo.m_blending = m_blending;
    copyTo.m_orientation = m_orientation;
    copyTo.m_texturesCountInAtlas = m_texturesCountInAtlas;
    copyTo.m_useAlphaTest = m_useAlphaTest;
    copyTo.m_lookVector = m_lookVector;
    copyTo.m_upVector = m_upVector;

    if(copyTo.m_SPKIrrQuadRenderer)
        delete copyTo.m_SPKIrrQuadRenderer;

    copyTo.m_SPKIrrQuadRenderer = nullptr;

    // note that m_SPKIrrQuadRenderer has to be recreated after calling this method
}

} // namespace app3D
} // namespace engine
