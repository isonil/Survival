#include "Def.hpp"

namespace engine
{

Def::Def()
    : m_defName{"[undefined]"},
      m_capitalizedLabelCached{}
{
}

void Def::expose(DataFile::Node &node)
{
    node.var(m_defName, "defName");
    node.var(m_label, "label");
    node.var(m_description, "description");

    // if it's cached, then recache it
    if(m_capitalizedLabelCached)
        m_capitalizedLabel = createCapitalizedLabel();
}

void Def::onLoadedAllDefs(DefDatabase &defDatabase)
{
}

const std::string &Def::getDefName() const
{
    return m_defName;
}

const std::string &Def::getLabel() const
{
    return m_label;
}

const std::string &Def::getDescription() const
{
    return m_description;
}

const std::string &Def::getCapitalizedLabel()
{
    if(!m_capitalizedLabelCached) {
        m_capitalizedLabel = createCapitalizedLabel();
        m_capitalizedLabelCached = true;
    }

    return m_capitalizedLabel;
}

std::string Def::getCapitalizedLabel() const
{
    if(m_capitalizedLabelCached)
        return m_capitalizedLabel;

    return createCapitalizedLabel();
}

std::string Def::createCapitalizedLabel() const
{
    auto ret = m_label;

    if(!ret.empty() && std::islower(ret[0]))
        ret[0] = std::toupper(ret[0]);

    return ret;
}

} // namespace engine
