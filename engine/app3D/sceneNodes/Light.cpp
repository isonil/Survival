#include "Light.hpp"

#include "../../util/Exception.hpp"
#include "../../util/AppTime.hpp"
#include "../managers/ShadersManager.hpp"
#include "../defs/LightDef.hpp"
#include "../Device.hpp"
#include "../IrrlichtConversions.hpp"

namespace engine
{
namespace app3D
{

Light::Light(const std::shared_ptr <LightDef> &lightDef, const std::weak_ptr <Device> &device)
    : SceneNode{device},
      m_currentRender{},
      m_lightDef{lightDef},
      m_directionalLightDirection{static_cast <float> (1.0 / sqrt(3.0)),
                                  static_cast <float> (-1.0 / sqrt(3.0)),
                                  static_cast <float> (1.0 / sqrt(3.0))},
      m_radiusDistortion{},
      m_currentRadiusDistortionTarget{}
{
    if(!m_lightDef)
        throw Exception{"Light def is nullptr."};

    m_color = m_lightDef->getInitialColor();

    createRender();

    if(m_lightDef->isDirectional()) {
        auto &shadersManager = getDevice_slow().getShadersManager();
        shadersManager.setDirectionalLightDirection(m_directionalLightDirection);
        shadersManager.setDirectionalLightColor(m_color);
    }
}

void Light::dropIrrObjects()
{
    m_currentRender.lightNode = nullptr;
}

void Light::reloadIrrObjects()
{
    createRender();
}

bool Light::wantsEverUpdate() const
{
    E_DASSERT(m_lightDef, "Light def is nullptr.");

    // point lights need to be updated in order to inform ShadersManager about their existence,
    // and because they can use radius distortion
    return !m_lightDef->isDirectional();
}

void Light::update(const FloatVec3 &cameraPos, const AppTime &appTime)
{
    E_DASSERT(m_lightDef, "Light def is nullptr.");

    if(!m_lightDef->isDirectional()) {
        if(!Math::fuzzyCompare(m_lightDef->getMaxRadiusDistortion(), 0.f) || !Math::fuzzyCompare(m_radiusDistortion, 0.f))
            updateRadiusDistortion(appTime);

        if(m_currentRender.lightNode) {
            auto radius = getCurrentRadius();

            m_currentRender.lightNode->setRadius(radius);
            m_currentRender.lightNode->getLightData().Attenuation = {1.f, 0.f, k_quadricAttenuationConstant / (radius * radius)};
        }

        auto &shadersManager = getDevice_slow().getShadersManager();
        shadersManager.registerPointLight(*this);
    }
}

LightDef &Light::getDef() const
{
    E_DASSERT(m_lightDef, "Light def is nullptr.");

    return *m_lightDef;
}

void Light::setPosition(const FloatVec3 &pos)
{
    if(pos != m_pos) {
        m_pos = pos;

        E_DASSERT(m_lightDef, "Light def is nullptr.");

        if(!m_lightDef->isDirectional() && m_currentRender.lightNode) {
            const auto &irrPos = IrrlichtConversions::toVector(m_pos);
            m_currentRender.lightNode->setPosition(irrPos);
        }
    }
}

void Light::setDirectionalLightDirection(const FloatVec3 &dir)
{
    E_DASSERT(m_lightDef, "Light def is nullptr.");

    if(!m_lightDef->isDirectional())
        return;

    if(dir.isFuzzyZero())
        return;

    const auto &dirNormalized = dir.normalized();

    if(dirNormalized != m_directionalLightDirection) {
        m_directionalLightDirection = dirNormalized;

        if(m_currentRender.lightNode) {
            const auto &irrRot = IrrlichtConversions::toVector(m_directionalLightDirection).getHorizontalAngle();
            m_currentRender.lightNode->setRotation(irrRot);
        }

        getDevice_slow().getShadersManager().setDirectionalLightDirection(m_directionalLightDirection);
    }
}

void Light::setColor(const Color &color)
{
    if(color != m_color) {
        m_color = color;

        if(m_currentRender.lightNode) {
            const auto &irrColor = IrrlichtConversions::toColorf(m_color);

            auto &lightData = m_currentRender.lightNode->getLightData();
            lightData.DiffuseColor = irrColor;
            lightData.SpecularColor = irrColor;
        }

        E_DASSERT(m_lightDef, "Light def is nullptr.");

        if(m_lightDef->isDirectional())
            getDevice_slow().getShadersManager().setDirectionalLightColor(m_color);
    }
}

const FloatVec3 &Light::getPosition() const
{
    return m_pos;
}

const Color &Light::getColor() const
{
    return m_color;
}

float Light::getCurrentRadius() const
{
    E_DASSERT(m_lightDef, "Light def is nullptr.");

    if(m_radiusDistortion < 0.f)
        return m_lightDef->getRadius() / (1.f - m_radiusDistortion);
    else
        return m_lightDef->getRadius() * (1.f + m_radiusDistortion);
}

Light::~Light()
{
    TRACK;

    if(!deviceExpired())
        removeCurrentRender();
}

const float Light::k_quadricAttenuationConstant{1.4f};

void Light::createRender()
{
    TRACK;

    removeCurrentRender();

    const auto &irrPos = IrrlichtConversions::toVector(m_pos);
    const auto &irrColor = IrrlichtConversions::toColorf(m_color);

    auto &irrSceneManager = *getDevice_slow().getIrrDevice().getSceneManager();

    auto radius = getCurrentRadius();

    m_currentRender.lightNode = irrSceneManager.addLightSceneNode(irrSceneManager.getRootSceneNode(),
        irrPos,
        irrColor,
        radius);

    if(!m_currentRender.lightNode)
        throw Exception{"Could not add light scene node."};

    auto &lightData = m_currentRender.lightNode->getLightData();

    lightData.DiffuseColor = irrColor;
    lightData.SpecularColor = irrColor;

    E_DASSERT(m_lightDef, "Light def is nullptr.");

    if(m_lightDef->isDirectional()) {
        m_currentRender.lightNode->setLightType(irr::video::ELT_DIRECTIONAL);
        m_currentRender.lightNode->setPosition({0.f, 0.f, 1.f});

        const auto &irrRot = IrrlichtConversions::toVector(m_directionalLightDirection).getHorizontalAngle();
        m_currentRender.lightNode->setRotation(irrRot);
    }
    else {
        m_currentRender.lightNode->getLightData().Attenuation = {1.f, 0.f, k_quadricAttenuationConstant / (radius * radius)};
        m_currentRender.lightNode->setLightType(irr::video::ELT_POINT);
    }
}

void Light::removeCurrentRender()
{
    TRACK;

    if(m_currentRender.lightNode) {
        m_currentRender.lightNode->remove();
        m_currentRender.lightNode = nullptr;
    }
}

void Light::updateRadiusDistortion(const AppTime &appTime)
{
    E_DASSERT(m_lightDef, "Light def is nullptr.");

    auto maxRadiusDistortion = m_lightDef->getMaxRadiusDistortion();

    double step{maxRadiusDistortion * appTime.getDeltaAsSeconds() * 7.f};

    if(std::fabs(m_radiusDistortion - m_currentRadiusDistortionTarget) < step)
        m_currentRadiusDistortionTarget = Random::rangeInclusive(-maxRadiusDistortion, maxRadiusDistortion);
    else if(m_radiusDistortion > m_currentRadiusDistortionTarget)
        m_radiusDistortion -= step;
    else
        m_radiusDistortion += step;
}

} // namespace app3D
} // namespace engine
