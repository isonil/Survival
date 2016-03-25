#ifndef APP_ON_USED_ITEM_HPP
#define APP_ON_USED_ITEM_HPP

#include "engine/util/DataFile.hpp"
#include "ItemGathersResourcesProperties.hpp"

#include <memory>

namespace engine { namespace app3D { class SoundDef; } }

namespace app
{

class EffectDef;

class OnUsedItem : public engine::DataFile::Saveable
{
public:
    class UseAnimation : public engine::DataFile::Saveable
    {
    public:
        UseAnimation();

        void expose(engine::DataFile::Node &node) override;

        int getFPPAnimationIndex() const;
        float getTimeBetweenUseAndActualEffects() const;

    private:
        int m_FPPAnimationIndex;
        float m_timeBetweenUseAndActualEffects;
    };

    OnUsedItem();

    void expose(engine::DataFile::Node &node) override;

    bool hasSoundDef() const;
    const std::shared_ptr <engine::app3D::SoundDef> &getSoundDefPtr() const;
    float getMinDuration() const;
    bool isContinuousUse() const;
    bool isWeapon() const;
    bool isMelee() const;
    int getDealsDamage() const;
    float getRecoil() const;
    float getMinProjectilesSpreadAngle() const;
    int getRandomUseAnimationIndex(int tryNotToTakeThisIndex = -1) const;
    const std::vector <UseAnimation> &getUseAnimations() const;
    const ItemGathersResourcesProperties &getGathersResourcesProperties() const;
    bool hasEffectDef() const;
    EffectDef &getEffectDef() const;
    const std::shared_ptr <EffectDef> &getEffectDefPtr() const;

private:
    std::string m_soundDef_defName;
    std::shared_ptr <engine::app3D::SoundDef> m_soundDef;
    float m_minDuration;
    bool m_continuousUse;
    bool m_isMelee;
    int m_dealsDamage;
    float m_recoil;
    float m_minProjectilesSpreadAngle;
    std::vector <UseAnimation> m_useAnimations;
    ItemGathersResourcesProperties m_gathersResourcesProperties;
    std::string m_effectDef_defName;
    std::shared_ptr <EffectDef> m_effectDef;
};

} // namespace app

#endif // APP_ON_USED_ITEM_HPP
