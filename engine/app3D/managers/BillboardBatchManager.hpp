#ifndef ENGINE_APP_3D_BILLBOARD_BATCH_MANAGER_HPP
#define ENGINE_APP_3D_BILLBOARD_BATCH_MANAGER_HPP

#include "../../util/Trace.hpp"
#include "../../util/Rect.hpp"
#include "../../util/Vec2.hpp"
#include "../../util/Vec3.hpp"

#include <irrlicht/irrlicht.h>

#include <map>
#include <vector>

namespace engine { namespace app3D { namespace irrNodes { class BillboardBatch; } } }

namespace engine
{
namespace app3D
{

class Device;

class BillboardBatchManager : public Tracked <BillboardBatchManager>
{
public:
    BillboardBatchManager(Device &device);

    void registerBillboard(irr::video::ITexture &billboard, const std::string &batchTag);
    std::shared_ptr <int> addBillboard(irr::video::ITexture &billboard, const std::string &batchTag, bool horizontal);
    void setBillboardPosition(int index, const FloatVec3 &pos);
    void setBillboardScale(int index, const FloatVec2 &scale);
    void removeBillboard(int index);

private:
    struct RegisteredBillboard
    {
        int batchIndex{-1};
        FloatRect textureAtlasRect;
    };

    struct BatchedBillboardInfo
    {
        irr::video::ITexture *billboard{};
        std::string batchTag;
        std::shared_ptr <int> index;
        int batchIndex{-1};
        int batchSecondLevelIndex{-1};
        std::shared_ptr <int> batchedBillboardIndex;
        bool horizontal{};
    };

    void createBatches();
    bool packTextures(const std::vector <irr::video::ITexture *> &billboards, int from, int to, const std::string &batchTag);

    static const int k_maxBillboardsPerBatch;

    Device &m_device;
    std::map <std::pair <irr::video::ITexture *, std::string>, RegisteredBillboard> m_registeredBillboards;
    std::vector <BatchedBillboardInfo> m_batchedBillboards;
    std::vector <std::vector <irrNodes::BillboardBatch *>> m_batches;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_BILLBOARD_BATCH_MANAGER_HPP
