#include "SkillsRequirement.hpp"

#include "../../../Global.hpp"
#include "../../../Core.hpp"
#include "../../Character.hpp"
#include "engine/app3D/Device.hpp"
#include "engine/GUI/IGUIRenderer.hpp"
#include "engine/GUI/GUIManager.hpp"
#include "../PlayerComponent.hpp"
#include "Skills.hpp"

namespace app
{

SkillsRequirement::SkillsRequirement()
    : m_level{},
      m_constructingLevel{},
      m_shootingLevel{},
      m_craftingLevel{},
      m_electronicsLevel{}
{
}

void SkillsRequirement::expose(engine::DataFile::Node &node)
{
    node.var(m_level, "level", {});
    node.var(m_constructingLevel, "constructingLevel", {});
    node.var(m_shootingLevel, "shootingLevel", {});
    node.var(m_craftingLevel, "craftingLevel", {});
    node.var(m_electronicsLevel, "electronicsLevel", {});
}

bool SkillsRequirement::isMet(const Character &character) const
{
    if(!character.isPlayer())
        return false;

    const auto &skills = character.getPlayerComponent().getSkills();

    return skills.getLevel() >= m_level &&
           skills.getConstructingLevel() >= m_constructingLevel &&
           skills.getShootingLevel() >= m_shootingLevel &&
           skills.getCraftingLevel() >= m_craftingLevel &&
           skills.getElectronicsLevel() >= m_electronicsLevel;
}

float SkillsRequirement::getArbitrarySeriousnessFactor() const
{
    // this is arbitrarily picked value based on level requirements,
    // returned value should be greater if requirement is considered 'more serious'

    return std::max({m_level, m_constructingLevel, m_shootingLevel, m_craftingLevel, m_electronicsLevel}) +
           m_level * 0.2f +
           m_constructingLevel * 0.2f +
           m_shootingLevel * 0.2f +
           m_craftingLevel * 0.2f +
           m_electronicsLevel * 0.2f;
}

engine::RichText SkillsRequirement::toRichText(const Character &character) const
{
    engine::RichText text;

    if(!character.isPlayer()) {
        text << "[not a player]";
        return text;
    }

    const auto &skills = character.getPlayerComponent().getSkills();
    const engine::Color okColor{0.6f, 0.75f, 0.6f};
    const engine::Color notOkColor{0.9f, 0.2f, 0.2f};

    if(m_level) {
        if(!text.isEmpty())
            text << '\n';

        if(m_level <= skills.getLevel())
            text << okColor;
        else
            text << notOkColor;

        text << "Required level: " << m_level;
    }

    if(m_constructingLevel) {
        if(!text.isEmpty())
            text << '\n';

        if(m_constructingLevel <= skills.getConstructingLevel())
            text << okColor;
        else
            text << notOkColor;

        text << "Required constructing level: " << m_constructingLevel;
    }

    if(m_shootingLevel) {
        if(!text.isEmpty())
            text << '\n';

        if(m_shootingLevel <= skills.getShootingLevel())
            text << okColor;
        else
            text << notOkColor;

        text << "Required shooting level: " << m_shootingLevel;
    }

    if(m_craftingLevel) {
        if(!text.isEmpty())
            text << '\n';

        if(m_craftingLevel <= skills.getCraftingLevel())
            text << okColor;
        else
            text << notOkColor;

        text << "Required crafting level: " << m_craftingLevel;
    }

    if(m_electronicsLevel) {
        if(!text.isEmpty())
            text << '\n';

        if(m_electronicsLevel <= skills.getElectronicsLevel())
            text << okColor;
        else
            text << notOkColor;

        text << "Required electronics level: " << m_electronicsLevel;
    }

    return text;
}

} // namespace app
