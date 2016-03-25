#ifndef APP_SKILLS_REQUIREMENT_HPP
#define APP_SKILLS_REQUIREMENT_HPP

#include "engine/util/DataFile.hpp"
#include "engine/util/Vec2.hpp"
#include "engine/util/RichText.hpp"

namespace app
{

class Character;

class SkillsRequirement : public engine::DataFile::Saveable
{
public:
    SkillsRequirement();

    void expose(engine::DataFile::Node &node) override;

    bool isMet(const Character &character) const;
    float getArbitrarySeriousnessFactor() const;
    engine::RichText toRichText(const Character &character) const;

private:
    int m_level;
    int m_constructingLevel;
    int m_shootingLevel;
    int m_craftingLevel;
    int m_electronicsLevel;
};

} // namespace app

#endif // APP_SKILLS_REQUIREMENT_HPP

