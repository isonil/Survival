#ifndef ENGINE_APP_3D_I_IRRLICHT_OBJECTS_HOLDER_HPP
#define ENGINE_APP_3D_I_IRRLICHT_OBJECTS_HOLDER_HPP

namespace engine
{
namespace app3D
{

class IIrrlichtObjectsHolder
{
public:
    IIrrlichtObjectsHolder() = default;
    IIrrlichtObjectsHolder(const IIrrlichtObjectsHolder &) = delete;
    IIrrlichtObjectsHolder(IIrrlichtObjectsHolder &&) = default;

    IIrrlichtObjectsHolder &operator = (const IIrrlichtObjectsHolder &) = delete;
    IIrrlichtObjectsHolder &operator = (IIrrlichtObjectsHolder &&) = default;

    virtual void dropIrrObjects() = 0;
    virtual void reloadIrrObjects() = 0;

    virtual ~IIrrlichtObjectsHolder() = default;
};

} // namespace app3D
} // namespace engine

#endif // ENGINE_APP_3D_I_IRRLICHT_OBJECTS_HOLDER_HPP
