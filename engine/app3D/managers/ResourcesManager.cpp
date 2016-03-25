#include "ResourcesManager.hpp"

#include "../../util/Exception.hpp"
#include "../../util/LogManager.hpp"
#include "../../util/RectPacker.hpp"
#include "../ext/CGUITTFont.h"
#include "../sceneNodes/Model.hpp"
#include "../sceneNodes/Terrain.hpp"
#include "../sceneNodes/Light.hpp"
#include "../Device.hpp"

#include <sstream>

namespace engine
{
namespace app3D
{

ResourcesManager::ResourcesManager(Device &device, const Settings &settings)
    : m_device{device},
      m_settings{settings},
      m_meshBatchManager{device},
      m_billboardBatchManager{device},
      m_uniqueResourceID{},
      m_missingMeshMesh{},
      m_missingTextureTexture{},
      m_missingImageImage{},
      m_defaultFont{},
      m_whiteTexture{},
      m_blackTexture{},
      m_simplePlaneMesh{}
{
    reloadIrrObjects();
}

void ResourcesManager::dropIrrObjects()
{
    TRACK;

    E_INFO("Dropping Irrlicht resources from resources manager.");

    m_missingMeshMesh = nullptr;
    m_missingTextureTexture = nullptr;
    m_missingImageImage = nullptr;
    m_defaultFont = nullptr;
    m_whiteTexture = nullptr;
    m_blackTexture = nullptr;
    m_simplePlaneMesh = nullptr;
}

void ResourcesManager::reloadIrrObjects()
{
    TRACK;

    E_INFO("Reloading Irrlicht resources in resources manager.");

    m_missingMeshMesh = &loadIrrAnimatedMesh("missingMesh.obj");
    m_missingTextureTexture = &loadIrrTexture("missingTexture.png", true);
    m_missingImageImage = &loadIrrImage("missingImage.png");
    m_defaultFont = &loadIrrFont("defaultFont.ttf", 12);
    m_whiteTexture = &loadIrrTexture("white.png", true);
    m_blackTexture = &loadIrrTexture("black.png", true);

    m_simplePlaneMesh = m_device.getIrrDevice().getSceneManager()->addHillPlaneMesh("simplePlane",
        irr::core::dimension2df{1.f, 1.f},
        irr::core::dimension2du{1, 1},
        nullptr,
        0.f,
        irr::core::dimension2df{0.f, 0.f},
        irr::core::dimension2df{1.f, 1.f});

    if(!m_simplePlaneMesh)
        throw Exception{"Could not add simple plane mesh."};
}

MeshBatchManager &ResourcesManager::getMeshBatchManager()
{
    return m_meshBatchManager;
}

BillboardBatchManager &ResourcesManager::getBillboardBatchManager()
{
    return m_billboardBatchManager;
}

std::vector <FloatVec3> ResourcesManager::getMeshPoints(const std::string &meshPath, const std::string &preferredModPath)
{
    auto &mesh = loadIrrMesh(meshPath, preferredModPath);
    std::vector <FloatVec3> ret;

    for(irr::u32 i = 0; i < mesh.getMeshBufferCount(); ++i) {
        auto *meshBuffer = mesh.getMeshBuffer(i);

        E_DASSERT(meshBuffer->getVertexType() == irr::video::EVT_STANDARD, "Expected EVT_STANDARD vertex type.");

        for(irr::u32 j = 0; j < meshBuffer->getIndexCount(); ++j) {
            auto index = meshBuffer->getIndices()[j];

            if(index < meshBuffer->getVertexCount()) {
                const auto &pos = (static_cast <irr::video::S3DVertex*> (meshBuffer->getVertices()))[index].Pos;
                ret.emplace_back(pos.X, pos.Y, pos.Z);
            }
            else
                throw Exception{"Mesh \"" + meshPath + "\" uses vertex index out of bounds."};
        }
    }

    return ret;
}

std::string ResourcesManager::getPathToResource(const std::string &path, const std::string &preferredModPath) const
{
    const auto &fileSystem = *m_device.getIrrDevice().getFileSystem();

    const auto &firstTry = "mods/" + preferredModPath + '/' + path;

    if(fileSystem.existFile(firstTry.c_str()))
        return firstTry;

    for(const auto &elem : m_settings.mods.mods) {
        if(!elem.enabled)
            continue;

        const auto &nextTry = "mods/" + elem.path + '/' + path;

        if(fileSystem.existFile(nextTry.c_str()))
            return nextTry;
    }

    return firstTry;
}

irr::scene::IMesh &ResourcesManager::loadIrrMesh(const std::string &path, const std::string &preferredModPath)
{
    TRACK;

    irr::scene::IMesh *mesh{};

    if(!path.empty()) {
        mesh = tryLoadIrrMesh_internal("mods/" + preferredModPath + "/meshes/" + path);

        if(!mesh) {
            for(const auto &elem : m_settings.mods.mods) {
                if(!elem.enabled)
                    continue;

                mesh = tryLoadIrrMesh_internal("mods/" + elem.path + "/meshes/" + path);

                if(mesh)
                    break;
            }
        }
    }

    if(!mesh) {
        E_ERROR("Could not load mesh \"%s\".", path.c_str());

        if(!m_missingMeshMesh)
            throw Exception{"Missing mesh mesh is not loaded."};

        return *m_missingMeshMesh;
    }

    for(irr::u32 i = 0; i < mesh->getMeshBufferCount(); ++i) {
        processMeshMaterial(mesh->getMeshBuffer(i)->getMaterial());
    }

    return *mesh;
}

irr::scene::IAnimatedMesh &ResourcesManager::loadIrrAnimatedMesh(const std::string &path, const std::string &preferredModPath)
{
    TRACK;

    irr::scene::IAnimatedMesh *animatedMesh{};

    if(!path.empty()) {
        animatedMesh = tryLoadIrrAnimatedMesh_internal("mods/" + preferredModPath + "/meshes/" + path);

        if(!animatedMesh) {
            for(const auto &elem : m_settings.mods.mods) {
                if(!elem.enabled)
                    continue;

                animatedMesh = tryLoadIrrAnimatedMesh_internal("mods/" + elem.path + "/meshes/" + path);

                if(animatedMesh)
                    break;
            }
        }
    }

    if(!animatedMesh) {
        E_ERROR("Could not load animated mesh \"%s\".", path.c_str());

        if(!m_missingMeshMesh)
            throw Exception{"Missing mesh mesh is not loaded."};

        return *m_missingMeshMesh;
    }

    for(irr::u32 i = 0; i < animatedMesh->getMeshBufferCount(); ++i) {
        processMeshMaterial(animatedMesh->getMeshBuffer(i)->getMaterial());
    }

    return *animatedMesh;
}

irr::video::ITexture &ResourcesManager::loadIrrTexture(const std::string &path, bool generateMipMaps, const std::string &preferredModPath)
{
    TRACK;

    auto &irrDevice = m_device.getIrrDevice();
    auto &driver = *irrDevice.getVideoDriver();

    bool prevCreateMipMaps{driver.getTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS)};

    driver.setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, generateMipMaps);

    irr::video::ITexture *texture{};

    if(!path.empty()) {
        texture = tryLoadIrrTexture_internal("mods/" + preferredModPath + "/textures/" + path);

        if(!texture) {
            for(const auto &elem : m_settings.mods.mods) {
                if(!elem.enabled)
                    continue;

                texture = tryLoadIrrTexture_internal("mods/" + elem.path + "/textures/" + path);

                if(texture)
                    break;
            }
        }
    }

    driver.setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, prevCreateMipMaps);

    if(!texture) {
        E_ERROR("Could not load texture \"%s\".", path.c_str());

        if(!m_missingTextureTexture)
            throw Exception{"Missing texture texture is not loaded."};

        return *m_missingTextureTexture;
    }

    return *texture;
}

// TODO: no texture arrays supported
/*
irr::video::ITexture &ResourcesManager::loadIrrTextureArray(const std::vector <std::string> &texturePaths)
{
    TRACK;

    if(texturePaths.empty())
        throw Exception{"Can't load texture array with 0 textures."};

    auto &irrDevice = m_device.getIrrDevice();
    auto &driver = *irrDevice.getVideoDriver();

    irr::core::array <irr::io::path> irrTexturePaths;

    for(const auto &elem : texturePaths) {
        irrTexturePaths.push_back(elem.c_str());
    }

    irr::video::ITexture *textureArray{driver.getTexture(irrTexturePaths)};

    if(!textureArray) {
        std::string errPaths;

        for(size_t i = 0; i < texturePaths.size(); ++i) {
            errPaths += '\"' + texturePaths[i] + '\"';

            if(i != texturePaths.size() - 1)
                errPaths += ", ";
        }

        E_ERROR("Could not load texture array. Texture paths: %s.", errPaths.c_str());

        // TODO: return missing texture array texture
        throw Exception{"Missing texture array behavior not implemented."};
    }

    textureArray->lock(); // HACK (we have to lock and unlock to avoid OpenGL errors)
    textureArray->unlock(); // HACK

    return *textureArray;
}
*/

irr::video::IImage &ResourcesManager::loadIrrImage(const std::string &path, const std::string &preferredModPath)
{
    TRACK;

    irr::video::IImage *image{};

    if(!path.empty()) {
        image = tryLoadIrrImage_internal("mods/" + preferredModPath + "/textures/" + path);

        if(!image) {
            for(const auto &elem : m_settings.mods.mods) {
                if(!elem.enabled)
                    continue;

                image = tryLoadIrrImage_internal("mods/" + elem.path + "/textures/" + path);

                if(image)
                    break;
            }
        }
    }

    if(!image) {
        E_ERROR("Could not load image \"%s\".", path.c_str());

        if(!m_missingImageImage)
            throw Exception{"Missing image image is not loaded."};

        return *m_missingImageImage;
    }

    return *image;
}

irr::gui::CGUITTFont &ResourcesManager::loadIrrFont(const std::string &path, int size, const std::string &preferredModPath)
{
    TRACK;

    irr::gui::CGUITTFont *font{};

    if(!path.empty()) {
        font = tryLoadIrrFont_internal("mods/" + preferredModPath + "/fonts/" + path, size);

        if(!font) {
            for(const auto &elem : m_settings.mods.mods) {
                if(!elem.enabled)
                    continue;

                font = tryLoadIrrFont_internal("mods/" + elem.path + "/fonts/" + path, size);

                if(font)
                    break;
            }
        }
    }

    // FIXME: Handle removing font, handle it in reloadIrrObjects, dropIrrObjects and in destructor.

    if(!font) {
        E_ERROR("Could not load font \"%s\".", path.c_str());

        if(!m_defaultFont)
            throw Exception{"Default font is not loaded."};

        return *m_defaultFont;
    }

    return *font;
}

irr::video::ITexture *ResourcesManager::getPackedTexture(const std::vector <irr::video::ITexture*> &textures, std::vector <FloatRect> &outTexCoords) const
{
    TRACK;

    // FIXME: This function isn't exception safe.
    // TODO: Fix potential memory leaks (createImage allocates memory for new image).

    outTexCoords.clear();

    if(textures.empty())
        return &m_device.getResourcesManager().getWhiteTexture();

    irr::video::ITexture *newTexture{};
    irr::core::dimension2d <irr::u32> textureSize(k_packedTextureSize, k_packedTextureSize);
    int texturePadding{20};

    RectPacker packer{k_packedTextureSize};

    for(const auto &elem : textures) {
        if(!elem)
            throw Exception{"Texture is nullptr."};

        packer.add(elem->getSize().Width + texturePadding, elem->getSize().Height + texturePadding);
    }

    if(!packer.pack())
        return nullptr;

    const auto &rects = packer.getAll();

    E_RASSERT(rects.size() == textures.size(), "Tried to pack %d textures, but got %d rects.",
              static_cast <int> (textures.size()),
              static_cast <int> (rects.size()));

    auto &driver = *m_device.getIrrDevice().getVideoDriver();
    auto *packedImage = driver.createImage(textures[0]->getColorFormat(), textureSize);

    E_RASSERT(packedImage, "Created image is nullptr (tried to create texture with size: %d).", k_packedTextureSize);

    std::vector <irr::video::IImage *> textureImages;
    textureImages.resize(textures.size());

    for(size_t i = 0; i < textureImages.size(); ++i) {
        textureImages[i] = driver.createImage(textures[i], irr::core::vector2di{0, 0}, textures[i]->getSize());
        E_RASSERT(textureImages[i], "Created image is nullptr.");
    }

    outTexCoords.resize(textures.size());

    for(size_t i = 0; i < rects.size(); ++i) {
        auto *im = textureImages[i];

        int xPos{rects[i].rect.pos.x + texturePadding / 2};
        int yPos{rects[i].rect.pos.y + texturePadding / 2};

        im->copyTo(packedImage, {xPos, yPos});

        auto textureWidth = static_cast <int> (textures[i]->getSize().Width);
        auto textureHeight = static_cast <int> (textures[i]->getSize().Height);

        for(int j = 0; j < texturePadding / 2; ++j) {
            im->copyTo(packedImage, {xPos - j, yPos}, {0, 0, 1, textureHeight});
            im->copyTo(packedImage, {xPos + j + textureWidth, yPos}, {textureWidth - 1, 0, textureWidth, textureHeight});
            im->copyTo(packedImage, {xPos, yPos - j}, {0, 0, textureWidth, 1});
            im->copyTo(packedImage, {xPos, yPos + j + textureHeight}, {0, textureHeight - 1, textureWidth, textureHeight});
        }

        outTexCoords[i].pos.set(static_cast <float> (xPos) / textureSize.Width,
                                static_cast <float> (yPos) / textureSize.Height);

        outTexCoords[i].size.set(static_cast <float> (textures[i]->getSize().Width) / textureSize.Width,
                                 static_cast <float> (textures[i]->getSize().Height) / textureSize.Height);
    }

    irr::core::stringc textureName = "packedTexture";
    static int globalPackedTexturesCount;
    textureName += globalPackedTexturesCount;

    newTexture = driver.addTexture(textureName, packedImage);

    E_RASSERT(newTexture, "Added texture is nullptr.");

    newTexture->regenerateMipMapLevels();
    ++globalPackedTexturesCount;

    // debug write to file
    m_device.getIrrDevice().getVideoDriver()->writeImageToFile(packedImage, ("atlas" + std::to_string(globalPackedTexturesCount) + ".png").c_str());

    for(auto &elem : textureImages) {
        elem->drop();
    }

    packedImage->drop();

    return newTexture;
}

irr::scene::IMesh &ResourcesManager::getSimplePlaneMesh() const
{
    TRACK;

    if(!m_simplePlaneMesh)
        throw Exception{"Simple plane mesh is nullptr."};

    return *m_simplePlaneMesh;
}

irr::video::ITexture &ResourcesManager::getWhiteTexture() const
{
    TRACK;

    if(!m_whiteTexture)
        throw Exception{"White texture is not loaded."};

    return *m_whiteTexture;
}

irr::video::ITexture &ResourcesManager::getBlackTexture() const
{
    TRACK;

    if(!m_blackTexture)
        throw Exception{"Black texture is not loaded."};

    return *m_blackTexture;
}

void ResourcesManager::processMeshMaterial(irr::video::SMaterial &material)
{
    for(irr::u32 i = 0; i < irr::video::MATERIAL_MAX_TEXTURES; ++i) {
        material.TextureLayer[i].AnisotropicFilter = k_anisotropicFilterLevel;
        material.TextureLayer[i].BilinearFilter = true;
        material.TextureLayer[i].TrilinearFilter = false;
    }

    // HACK: .obj materials have ADD_COLOR material assigned by Irrlicht for some reason
    // (this is probably caused by map_d, but it's the only way to tell which material should be transparent)
    if(material.MaterialType == irr::video::EMT_TRANSPARENT_ADD_COLOR)
        material.MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;

    // depth sorting per-vertex is not supported, so we have to use REF
    if(material.MaterialType == irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL)
        material.MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;

    if(material.MaterialType == irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF) {
        // HACK: assuming all materials with alpha channel ref should not use backface culling
        material.BackfaceCulling = false;
        material.FrontfaceCulling = false;
    }
    else {
        material.BackfaceCulling = true;
        material.FrontfaceCulling = false;
    }

    material.GouraudShading = true;
}

irr::scene::IMesh *ResourcesManager::tryLoadIrrMesh_internal(const std::string &fullPath)
{
    auto &irrDevice = m_device.getIrrDevice();

    if(!irrDevice.getFileSystem()->existFile(fullPath.c_str()))
        return {};

    return irrDevice.getSceneManager()->getMesh(fullPath.c_str());
}

irr::scene::IAnimatedMesh *ResourcesManager::tryLoadIrrAnimatedMesh_internal(const std::string &fullPath)
{
    auto &irrDevice = m_device.getIrrDevice();

    if(!irrDevice.getFileSystem()->existFile(fullPath.c_str()))
        return {};

    return irrDevice.getSceneManager()->getMesh(fullPath.c_str());
}

irr::video::ITexture *ResourcesManager::tryLoadIrrTexture_internal(const std::string &fullPath)
{
    auto &irrDevice = m_device.getIrrDevice();

    if(!irrDevice.getFileSystem()->existFile(fullPath.c_str()))
        return {};

    return irrDevice.getVideoDriver()->getTexture(fullPath.c_str());
}

irr::video::IImage *ResourcesManager::tryLoadIrrImage_internal(const std::string &fullPath)
{
    auto &irrDevice = m_device.getIrrDevice();

    if(!irrDevice.getFileSystem()->existFile(fullPath.c_str()))
        return {};

    return irrDevice.getVideoDriver()->createImageFromFile(fullPath.c_str());
}

irr::gui::CGUITTFont *ResourcesManager::tryLoadIrrFont_internal(const std::string &fullPath, int size)
{
    auto &irrDevice = m_device.getIrrDevice();

    if(!irrDevice.getFileSystem()->existFile(fullPath.c_str()))
        return {};

    return irr::gui::CGUITTFont::createTTFont(irrDevice.getGUIEnvironment(), fullPath.c_str(), size);
}

const int ResourcesManager::k_packedTextureSize{2048};
const int ResourcesManager::k_anisotropicFilterLevel{8};

} // namespace app3D
} // namespace engine
