#ifndef ENGINE_DEF_HPP
#define ENGINE_DEF_HPP

#include "Trace.hpp"
#include "DataFile.hpp"

#include <string>

namespace engine
{

class DefDatabase;

class Def : public DataFile::Saveable, public Tracked <Def>
{
public:
    Def();

    void expose(DataFile::Node &node) override;

    virtual void onLoadedAllDefs(DefDatabase &defDatabase);

    const std::string &getDefName() const;
    const std::string &getLabel() const;
    const std::string &getDescription() const;
    const std::string &getCapitalizedLabel();
    std::string getCapitalizedLabel() const;

    virtual ~Def() = default;

private:
    std::string createCapitalizedLabel() const;

    std::string m_defName;
    std::string m_label;
    std::string m_description;
    std::string m_capitalizedLabel;
    bool m_capitalizedLabelCached;
};

} // namespace engine

#endif // ENGINE_DEF_HPP
