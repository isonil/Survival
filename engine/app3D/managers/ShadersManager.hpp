#ifndef ENGINE_APP_3D_SHADERS_MANAGER_HPP
#define ENGINE_APP_3D_SHADERS_MANAGER_HPP

#include "../../util/Trace.hpp"
#include "../../util/Vec3.hpp"
#include "../../util/Color.hpp"
#include "../Settings.hpp"

#include <QElapsedTimer>
#include <irrlicht/irrlicht.h>

#include <string>

namespace engine
{
namespace app3D
{

class Device;
class Light;

class DefaultShaderCallback : public irr::video::IShaderConstantSetCallBack
{
public:
    DefaultShaderCallback(Device &device);

private:
    virtual void OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData);

    Device &m_device;
};

class DeferredShaderCallback : public irr::video::IShaderConstantSetCallBack
{
public:
    DeferredShaderCallback(Device &device);

private:
    virtual void OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData);

    Device &m_device;
};

class TerrainShaderCallback : public irr::video::IShaderConstantSetCallBack
{
public:
    TerrainShaderCallback(Device &device);

private:
    virtual void OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData);

    Device &m_device;
};

class TerrainDeferredShaderCallback : public irr::video::IShaderConstantSetCallBack
{
public:
    TerrainDeferredShaderCallback(Device &device);

private:
    virtual void OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData);

    Device &m_device;
};

class FlatTerrainShaderCallback : public irr::video::IShaderConstantSetCallBack
{
public:
    FlatTerrainShaderCallback(Device &device);

private:
    virtual void OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData);

    Device &m_device;
};

class FlatTerrainDeferredShaderCallback : public irr::video::IShaderConstantSetCallBack
{
public:
    FlatTerrainDeferredShaderCallback(Device &device);

private:
    virtual void OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData);

    Device &m_device;
};

class LightToLightMapShaderCallback : public irr::video::IShaderConstantSetCallBack
{
public:
    LightToLightMapShaderCallback(Device &device);

private:
    virtual void OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData);

    Device &m_device;
};

class WaterShaderCallback : public irr::video::IShaderConstantSetCallBack
{
public:
    WaterShaderCallback(Device &device);

private:
    virtual void OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData);

    Device &m_device;
};

class WhiteShaderCallback : public irr::video::IShaderConstantSetCallBack
{
public:
    WhiteShaderCallback(Device &device);

private:
    virtual void OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData);

    Device &m_device;
};

class OutlineShaderCallback : public irr::video::IShaderConstantSetCallBack
{
public:
    OutlineShaderCallback(Device &device);

private:
    virtual void OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData);

    Device &m_device;
};

class SkyShaderCallback : public irr::video::IShaderConstantSetCallBack
{
public:
    SkyShaderCallback(Device &device);

private:
    virtual void OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData);

    Device &m_device;
};

class ShadersManager : public Tracked <ShadersManager>
{
public:
    struct PointLight
    {
        float distanceSqToCamera{};
        float radius{};

        FloatVec3 pos;
        Color color;
        float quadricAttenuation{};
    };

    ShadersManager(Device &device, const Settings &settings);
    ShadersManager(const ShadersManager &) = delete;

    ShadersManager &operator = (const ShadersManager &) = delete;

    irr::video::E_MATERIAL_TYPE getDefaultShaderMaterialType() const;
    irr::video::E_MATERIAL_TYPE getDeferredShaderMaterialType() const;
    irr::video::E_MATERIAL_TYPE getTerrainShaderMaterialType() const;
    irr::video::E_MATERIAL_TYPE getTerrainDeferredShaderMaterialType() const;
    irr::video::E_MATERIAL_TYPE getFlatTerrainShaderMaterialType() const;
    irr::video::E_MATERIAL_TYPE getFlatTerrainDeferredShaderMaterialType() const;
    irr::video::E_MATERIAL_TYPE getLightToLightMapShaderMaterialType() const;
    irr::video::E_MATERIAL_TYPE getWaterShaderMaterialType() const;
    irr::video::E_MATERIAL_TYPE getWhiteShaderMaterialType() const;
    irr::video::E_MATERIAL_TYPE getOutlineShaderMaterialType() const;
    irr::video::E_MATERIAL_TYPE getSkyShaderMaterialType() const;

    float getElapsedMilliseconds() const;
    void setDirectionalLightDirection(const FloatVec3 &lightDir);
    void setDirectionalLightColor(const Color &color);
    void setOutlineShaderOutlineColor(const Color &color);
    void setSkyColor(const Color &color);
    const FloatVec3 &getDirectionalLightDirection() const;
    const Color &getDirectionalLightColor() const;
    const Color &getOutlineShaderOutlineColor() const;
    const Color &getSkyColor() const;
    void clearPointLights();
    void registerPointLight(const Light &light);
    const std::vector <PointLight> &getPointLights() const;

    static const int k_maxActivePointLights;

private:
    std::string loadShader(const std::string &path, const std::string &preferredModPath = "core");

    void compileAllShaders();
    irr::s32 compileShader(const std::string &vertexShader, const std::string &fragmentShader, irr::video::IShaderConstantSetCallBack &callback, irr::video::E_MATERIAL_TYPE baseMaterial = irr::video::EMT_SOLID);

    Device &m_device;
    const Settings m_settings;
    QElapsedTimer m_timer;

    irr::video::E_MATERIAL_TYPE m_defaultShaderMaterialType;
    irr::video::E_MATERIAL_TYPE m_deferredShaderMaterialType;
    irr::video::E_MATERIAL_TYPE m_terrainShaderMaterialType;
    irr::video::E_MATERIAL_TYPE m_terrainDeferredShaderMaterialType;
    irr::video::E_MATERIAL_TYPE m_flatTerrainShaderMaterialType;
    irr::video::E_MATERIAL_TYPE m_flatTerrainDeferredShaderMaterialType;
    irr::video::E_MATERIAL_TYPE m_lightToLightMapShaderMaterialType;
    irr::video::E_MATERIAL_TYPE m_waterShaderMaterialType;
    irr::video::E_MATERIAL_TYPE m_whiteShaderMaterialType;
    irr::video::E_MATERIAL_TYPE m_outlineShaderMaterialType;
    irr::video::E_MATERIAL_TYPE m_skyShaderMaterialType;

    DefaultShaderCallback m_defaultShaderCallback;
    DeferredShaderCallback m_deferredShaderCallback;
    TerrainShaderCallback m_terrainShaderCallback;
    TerrainDeferredShaderCallback m_terrainDeferredShaderCallback;
    FlatTerrainShaderCallback m_flatTerrainShaderCallback;
    FlatTerrainDeferredShaderCallback m_flatTerrainDeferredShaderCallback;
    LightToLightMapShaderCallback m_lightToLightMapShaderCallback;
    WaterShaderCallback m_waterShaderCallback;
    WhiteShaderCallback m_whiteShaderCallback;
    OutlineShaderCallback m_outlineShaderCallback;
    SkyShaderCallback m_skyShaderCallback;

    FloatVec3 m_directionalLightDirection;
    Color m_directionalLightColor;
    Color m_outlineShaderOutlineColor;
    Color m_skyColor;

    std::vector <PointLight> m_pointLights;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_SHADERS_MANAGER_HPP
