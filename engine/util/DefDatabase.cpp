#include "DefDatabase.hpp"

namespace engine
{

void DefDatabase::callOnLoadedAllDefs()
{
    for(auto &elem : m_defs) {
        E_DASSERT(elem.second, "Def is nullptr.");
        elem.second->onLoadedAllDefs(*this);
    }
}

void DefDatabase::dropAllDefs()
{
    TRACK;

    E_INFO("Dropping %d defs.", static_cast <int> (m_defs.size()));

    m_defs.clear();
}

const std::string DefDatabase::k_directoryScanDataFileExtension = "yaml";

} // namespace engine
