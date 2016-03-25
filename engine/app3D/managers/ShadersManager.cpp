#include "ShadersManager.hpp"

#include "../../util/Exception.hpp"
#include "../../util/LogManager.hpp"
#include "../sceneNodes/Light.hpp"
#include "../defs/LightDef.hpp"
#include "../Device.hpp"
#include "SceneManager.hpp"

namespace engine
{
namespace app3D
{

// TODO: use getPixelShaderConstantID and getVertexShaderConstantID

DefaultShaderCallback::DefaultShaderCallback(Device &device)
    : m_device{device}
{
}

void DefaultShaderCallback::OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData)
{
    irr::s32 layer0{};

    services->setPixelShaderConstant("colorMap", &layer0, 1);
}

DeferredShaderCallback::DeferredShaderCallback(Device &device)
    : m_device{device}
{
}

void DeferredShaderCallback::OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData)
{
    irr::s32 layer0{};

    services->setPixelShaderConstant("colorMap", &layer0, 1);
}

TerrainShaderCallback::TerrainShaderCallback(Device &device)
    : m_device{device}
{
}

void TerrainShaderCallback::OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData)
{
    irr::s32 layer0{};
    irr::s32 layer1{1};
    irr::s32 layer2{2};
    irr::s32 layer3{3};
    irr::s32 layer4{4};
    irr::s32 layer5{5};
    irr::s32 layer6{6};

    auto &shadersManager = m_device.getShadersManager();

    const auto &lightDir = shadersManager.getDirectionalLightDirection();
    const auto &lightCol = shadersManager.getDirectionalLightColor();
    const auto &fogCol = m_device.getFogColor();
    float milliseconds{shadersManager.getElapsedMilliseconds()};

    float lightDir_arr[3] = {
        lightDir.x, lightDir.y, lightDir.z
    };

    float lightCol_arr[3] = {
        lightCol.r, lightCol.g, lightCol.b
    };

    float fogCol_arr[3] = {
        fogCol.r, fogCol.g, fogCol.b
    };

    float ambientLight{m_device.getSceneManager().getAmbientLight()};

    services->setPixelShaderConstant("texture1", &layer0, 1);
    services->setPixelShaderConstant("texture2", &layer1, 1);
    services->setPixelShaderConstant("texture3", &layer2, 1);
    services->setPixelShaderConstant("slopeTexture", &layer3, 1);
    services->setPixelShaderConstant("splatMap", &layer4, 1);
    services->setPixelShaderConstant("normalMap", &layer5, 1);
    services->setPixelShaderConstant("causticsTexture", &layer6, 1);
    services->setPixelShaderConstant("ambientLight", &ambientLight, 1);
    services->setPixelShaderConstant("directionalLightDir", lightDir_arr, 3);
    services->setPixelShaderConstant("directionalLightColor", lightCol_arr, 3);
    services->setPixelShaderConstant("fogColor", fogCol_arr, 3);
    services->setPixelShaderConstant("time", &milliseconds, 1);

    // vertex shader

    const auto &pointLights = shadersManager.getPointLights();

    for(int i = 0; i < ShadersManager::k_maxActivePointLights; ++i) {
        float pos_arr[3]{};
        float color_arr[3]{};
        float quadricAttenuation{1.f};

        const auto &baseStr = "pointLights[" + std::to_string(i) + "].";

        if(i < static_cast <int> (pointLights.size())) {
            pos_arr[0] = pointLights[i].pos.x;
            pos_arr[1] = pointLights[i].pos.y;
            pos_arr[2] = pointLights[i].pos.z;

            color_arr[0] = pointLights[i].color.r;
            color_arr[1] = pointLights[i].color.g;
            color_arr[2] = pointLights[i].color.b;

            quadricAttenuation = pointLights[i].quadricAttenuation;
        }

        services->setVertexShaderConstant((baseStr + "pos").c_str(), pos_arr, 3);
        services->setVertexShaderConstant((baseStr + "color").c_str(), color_arr, 3);
        services->setVertexShaderConstant((baseStr + "quadricAttenuation").c_str(), &quadricAttenuation, 1);
    }
}

TerrainDeferredShaderCallback::TerrainDeferredShaderCallback(Device &device)
    : m_device{device}
{
}

void TerrainDeferredShaderCallback::OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData)
{
    irr::s32 layer0{};
    irr::s32 layer1{1};
    irr::s32 layer2{2};
    irr::s32 layer3{3};

    services->setPixelShaderConstant("splatMap", &layer0, 1);
    services->setPixelShaderConstant("textures", &layer1, 1);
    services->setPixelShaderConstant("normalMap", &layer2, 1);
    services->setPixelShaderConstant("occlusionMap", &layer3, 1);
}

FlatTerrainShaderCallback::FlatTerrainShaderCallback(Device &device)
    : m_device{device}
{
}

void FlatTerrainShaderCallback::OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData)
{
    irr::s32 layer0{};
    irr::s32 layer1{1};

    auto &shadersManager = m_device.getShadersManager();

    const auto &lightDir = shadersManager.getDirectionalLightDirection();
    const auto &lightCol = shadersManager.getDirectionalLightColor();
    const auto &fogCol = m_device.getFogColor();
    float milliseconds{shadersManager.getElapsedMilliseconds()};

    float ambientLight{m_device.getSceneManager().getAmbientLight()};

    FloatVec3 upVector{0.f, 1.f, 0.f};
    const auto &precomputedLighting = lightCol * (1.f - ambientLight) * std::max(upVector.dotProduct(-lightDir), 0.f) + ambientLight;

    float precomputedLighting_arr[3] = {
        precomputedLighting.r, precomputedLighting.g, precomputedLighting.b
    };

    float fogCol_arr[3] = {
        fogCol.r, fogCol.g, fogCol.b
    };

    services->setPixelShaderConstant("texture1", &layer0, 1);
    services->setPixelShaderConstant("causticsTexture", &layer1, 1);
    services->setPixelShaderConstant("precomputedLighting", precomputedLighting_arr, 3);
    services->setPixelShaderConstant("fogColor", fogCol_arr, 3);
    services->setPixelShaderConstant("time", &milliseconds, 1);
}

FlatTerrainDeferredShaderCallback::FlatTerrainDeferredShaderCallback(Device &device)
    : m_device{device}
{
}

void FlatTerrainDeferredShaderCallback::OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData)
{
    // TODO: deferred rendering
}

LightToLightMapShaderCallback::LightToLightMapShaderCallback(Device &device)
    : m_device{device}
{
}

void LightToLightMapShaderCallback::OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData)
{
    irr::s32 layer0{};
    irr::s32 layer1{1};

    services->setPixelShaderConstant("normalMap", &layer0, 1);
    services->setPixelShaderConstant("posMap", &layer1, 1);
}

WaterShaderCallback::WaterShaderCallback(Device &device)
    : m_device{device}
{
}

void WaterShaderCallback::OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData)
{
    irr::s32 layer0{};
    irr::s32 layer1{1};
    irr::s32 layer2{2};

    const auto &fogCol = m_device.getFogColor();

    float fogCol_arr[3] = {
        fogCol.r, fogCol.g, fogCol.b
    };

    auto &shadersManager = m_device.getShadersManager();

    const auto &lightDir = shadersManager.getDirectionalLightDirection();
    const auto &lightCol = shadersManager.getDirectionalLightColor();
    float ambientLight{m_device.getSceneManager().getAmbientLight()};

    FloatVec3 upVector{0.f, 1.f, 0.f};
    const auto &precomputedLighting = lightCol * (1.f - ambientLight) * std::max(upVector.dotProduct(-lightDir), 0.f) + ambientLight;

    float precomputedLighting_arr[3] = {
        precomputedLighting.r, precomputedLighting.g, precomputedLighting.b
    };

    float milliseconds{shadersManager.getElapsedMilliseconds()};

    services->setPixelShaderConstant("waterTexture", &layer0, 1);
    services->setPixelShaderConstant("terrainHeightMap", &layer1, 1);
    services->setPixelShaderConstant("foamTexture", &layer2, 1);
    services->setPixelShaderConstant("time", &milliseconds, 1);
    services->setPixelShaderConstant("fogColor", fogCol_arr, 3);
    services->setPixelShaderConstant("precomputedLighting", precomputedLighting_arr, 3);
}

WhiteShaderCallback::WhiteShaderCallback(Device &device)
    : m_device{device}
{
}

void WhiteShaderCallback::OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData)
{
    irr::s32 layer0{};

    services->setPixelShaderConstant("texture", &layer0, 1);
}

OutlineShaderCallback::OutlineShaderCallback(Device &device)
    : m_device{device}
{
}

void OutlineShaderCallback::OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData)
{
    irr::s32 layer0{};

    const auto &size = m_device.getScreenSize();

    float sampleStep[2] = {
        1.f / size.x * 2.f,
        1.f / size.y * 2.f
    };

    const auto &color = m_device.getShadersManager().getOutlineShaderOutlineColor();

    float color_arr[3] = {
        color.r,
        color.g,
        color.b
    };

    services->setPixelShaderConstant("texture", &layer0, 1);
    services->setPixelShaderConstant("sampleStep", sampleStep, 2);
    services->setPixelShaderConstant("color", color_arr, 3);
}

SkyShaderCallback::SkyShaderCallback(Device &device)
    : m_device{device}
{
}

void SkyShaderCallback::OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData)
{
    irr::s32 layer0{};

    const auto &color = m_device.getShadersManager().getSkyColor();

    float color_arr[3] = {
        color.r,
        color.g,
        color.b
    };

    services->setPixelShaderConstant("texture", &layer0, 1);
    services->setPixelShaderConstant("color", color_arr, 3);
}

ShadersManager::ShadersManager(Device &device, const Settings &settings)
    : m_device{device},
      m_settings{settings},
      m_defaultShaderMaterialType{irr::video::EMT_SOLID},
      m_deferredShaderMaterialType{irr::video::EMT_SOLID},
      m_terrainShaderMaterialType{irr::video::EMT_SOLID},
      m_terrainDeferredShaderMaterialType{irr::video::EMT_SOLID},
      m_flatTerrainShaderMaterialType{irr::video::EMT_SOLID},
      m_flatTerrainDeferredShaderMaterialType{irr::video::EMT_SOLID},
      m_lightToLightMapShaderMaterialType{irr::video::EMT_SOLID},
      m_waterShaderMaterialType{irr::video::EMT_SOLID},
      m_whiteShaderMaterialType{irr::video::EMT_SOLID},
      m_outlineShaderMaterialType{irr::video::EMT_SOLID},
      m_skyShaderMaterialType{irr::video::EMT_SOLID},
      m_defaultShaderCallback{device},
      m_deferredShaderCallback{device},
      m_terrainShaderCallback{device},
      m_terrainDeferredShaderCallback{device},
      m_flatTerrainShaderCallback{device},
      m_flatTerrainDeferredShaderCallback{device},
      m_lightToLightMapShaderCallback{device},
      m_waterShaderCallback{device},
      m_whiteShaderCallback{device},
      m_outlineShaderCallback{device},
      m_skyShaderCallback{device},
      m_directionalLightDirection{static_cast <float> (1.0 / sqrt(3.0)),
                                  static_cast <float> (-1.0 / sqrt(3.0)),
                                  static_cast <float> (1.0 / sqrt(3.0))},
      m_directionalLightColor{Color::k_white},
      m_outlineShaderOutlineColor{Color::k_white},
      m_skyColor{m_device.getFogColor()}
{
    compileAllShaders();

    m_timer.start();
}

irr::video::E_MATERIAL_TYPE ShadersManager::getDefaultShaderMaterialType() const
{
    return m_defaultShaderMaterialType;
}

irr::video::E_MATERIAL_TYPE ShadersManager::getDeferredShaderMaterialType() const
{
    return m_deferredShaderMaterialType;
}

irr::video::E_MATERIAL_TYPE ShadersManager::getTerrainShaderMaterialType() const
{
    return m_terrainShaderMaterialType;
}

irr::video::E_MATERIAL_TYPE ShadersManager::getTerrainDeferredShaderMaterialType() const
{
    return m_terrainDeferredShaderMaterialType;
}

irr::video::E_MATERIAL_TYPE ShadersManager::getFlatTerrainShaderMaterialType() const
{
    return m_flatTerrainShaderMaterialType;
}

irr::video::E_MATERIAL_TYPE ShadersManager::getFlatTerrainDeferredShaderMaterialType() const
{
    return m_flatTerrainDeferredShaderMaterialType;
}

irr::video::E_MATERIAL_TYPE ShadersManager::getLightToLightMapShaderMaterialType() const
{
    return m_lightToLightMapShaderMaterialType;
}

irr::video::E_MATERIAL_TYPE ShadersManager::getWaterShaderMaterialType() const
{
    return m_waterShaderMaterialType;
}

irr::video::E_MATERIAL_TYPE ShadersManager::getWhiteShaderMaterialType() const
{
    return m_whiteShaderMaterialType;
}

irr::video::E_MATERIAL_TYPE ShadersManager::getOutlineShaderMaterialType() const
{
    return m_outlineShaderMaterialType;
}

irr::video::E_MATERIAL_TYPE ShadersManager::getSkyShaderMaterialType() const
{
    return m_skyShaderMaterialType;
}

float ShadersManager::getElapsedMilliseconds() const
{
    return (m_timer.nsecsElapsed() / 1000) / 1000.0;
}

void ShadersManager::setDirectionalLightDirection(const FloatVec3 &lightDir)
{
    m_directionalLightDirection = lightDir;
}

void ShadersManager::setDirectionalLightColor(const Color &color)
{
    m_directionalLightColor = color;
}

void ShadersManager::setOutlineShaderOutlineColor(const Color &color)
{
    m_outlineShaderOutlineColor = color;
}

void ShadersManager::setSkyColor(const Color &color)
{
    m_skyColor = color;
}

const FloatVec3 &ShadersManager::getDirectionalLightDirection() const
{
    return m_directionalLightDirection;
}

const Color &ShadersManager::getDirectionalLightColor() const
{
    return m_directionalLightColor;
}

const Color &ShadersManager::getOutlineShaderOutlineColor() const
{
    return m_outlineShaderOutlineColor;
}

const Color &ShadersManager::getSkyColor() const
{
    return m_skyColor;
}

void ShadersManager::clearPointLights()
{
    m_pointLights.clear();
}

void ShadersManager::registerPointLight(const Light &light)
{
    if(light.getDef().isDirectional()) {
        E_WARNING("Tried to register directional light as point light.");
        return;
    }

    const auto &cameraPos = m_device.getSceneManager().getCameraPosition();

    PointLight pointLight;

    auto radius = light.getCurrentRadius();

    pointLight.distanceSqToCamera = light.getPosition().getDistanceSq(cameraPos);
    pointLight.radius = radius;
    pointLight.pos = light.getPosition();
    pointLight.color = light.getColor();
    pointLight.quadricAttenuation = Light::k_quadricAttenuationConstant / (radius * radius);

    m_pointLights.emplace_back(std::move(pointLight));

    // sorting each time an element is inserted seems to be a bad idea,
    // but since (for now) we're only limited to only a few active lights (forward rendering),
    // it doesn't matter that much, and it is probably still better than using std::set anyway
    // (TODO: insert(std::upper_bound))
    std::sort(m_pointLights.begin(), m_pointLights.end(), [](const auto &lhs, const auto &rhs) {
        auto radius1 = -lhs.radius;
        auto radius2 = -rhs.radius;

        return std::tie(lhs.distanceSqToCamera, radius1) <
               std::tie(rhs.distanceSqToCamera, radius2);
    });

    if(static_cast <int> (m_pointLights.size()) > k_maxActivePointLights)
        m_pointLights.resize(k_maxActivePointLights);
}

const std::vector <ShadersManager::PointLight> &ShadersManager::getPointLights() const
{
    return m_pointLights;
}

const int ShadersManager::k_maxActivePointLights{4};

std::string ShadersManager::loadShader(const std::string &path, const std::string &preferredModPath)
{
    std::ifstream in;

    in.open("mods/" + preferredModPath + "/shaders/" + path);

    if(!in.is_open()) {
        for(const auto &elem : m_settings.mods.mods) {
            if(!elem.enabled)
                continue;

            in.open("mods/" + elem.path + "/shaders/" + path);

            if(in.is_open())
                break;
        }
    }

    if(!in.is_open())
        throw Exception{"Could not open shader file '" + path + "'."};

    std::string shader;

    for(std::string line; std::getline(in, line);) {
        shader += line + '\n';
    }

    return shader;
}

void ShadersManager::compileAllShaders()
{
    TRACK;

    E_INFO("Compiling shaders.");

    irr::s32 shaderID{-1};

    E_INFO("Compiling terrain shader.");

    shaderID = compileShader(
        loadShader("terrain.vert"),
        loadShader("terrain.frag"),
        m_terrainShaderCallback
    );

    m_terrainShaderMaterialType = static_cast <irr::video::E_MATERIAL_TYPE> (shaderID);

    E_INFO("Compiling flat terrain shader.");

    shaderID = compileShader(
        loadShader("flatTerrain.vert"),
        loadShader("flatTerrain.frag"),
        m_flatTerrainShaderCallback
    );

    m_flatTerrainShaderMaterialType = static_cast <irr::video::E_MATERIAL_TYPE> (shaderID);

    E_INFO("Compiling water shader.");

    shaderID = compileShader(
        loadShader("water.vert"),
        loadShader("water.frag"),
        m_waterShaderCallback,
        irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL
    );

    m_waterShaderMaterialType = static_cast <irr::video::E_MATERIAL_TYPE> (shaderID);

    E_INFO("Compiling white shader.");

    shaderID = compileShader(
        loadShader("white.vert"),
        loadShader("white.frag"),
        m_whiteShaderCallback,
        irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF
    );

    m_whiteShaderMaterialType = static_cast <irr::video::E_MATERIAL_TYPE> (shaderID);

    E_INFO("Compiling outline shader.");

    shaderID = compileShader(
        loadShader("outline.vert"),
        loadShader("outline.frag"),
        m_outlineShaderCallback,
        irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL
    );

    m_outlineShaderMaterialType = static_cast <irr::video::E_MATERIAL_TYPE> (shaderID);

    E_INFO("Compiling sky shader.");

    shaderID = compileShader(
        loadShader("sky.vert"),
        loadShader("sky.frag"),
        m_skyShaderCallback
    );

    m_skyShaderMaterialType = static_cast <irr::video::E_MATERIAL_TYPE> (shaderID);

    E_INFO("All shaders compiled.");
}

irr::s32 ShadersManager::compileShader(const std::string &vertexShader, const std::string &fragmentShader, irr::video::IShaderConstantSetCallBack &callback, irr::video::E_MATERIAL_TYPE baseMaterial)
{
    irr::s32 shaderID{m_device.getIrrDevice().getVideoDriver()->getGPUProgrammingServices()->addHighLevelShaderMaterial(
        vertexShader.c_str(),
        "main",
        irr::video::EVST_VS_1_1,
        fragmentShader.c_str(),
        "main",
        irr::video::EPST_PS_1_1,
        &callback,
        baseMaterial)};

    if(shaderID < 0)
        throw Exception{"Could not compile shader."};

    return shaderID;
}

} // namespace app3D
} // namespace engine
