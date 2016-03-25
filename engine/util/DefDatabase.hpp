#ifndef ENGINE_DEF_DATABASE_HPP
#define ENGINE_DEF_DATABASE_HPP

#include "Trace.hpp"
#include "Def.hpp"
#include "DataFile.hpp"

#include <QDirIterator>

#include <string>
#include <memory>
#include <unordered_map>

namespace engine
{

class DefDatabase : public Tracked <DefDatabase>
{
public:
    template <class T> struct DefsList : DataFile::Saveable
    {
        void expose(DataFile::Node &node) override;

        std::vector <T> defs;
    };

    DefDatabase() = default;
    DefDatabase(const DefDatabase &) = delete;

    DefDatabase &operator = (const DefDatabase &) = delete;

    template <class T> std::shared_ptr <T> getDef(const std::string &defName) const;
    template <class T> std::shared_ptr <T> tryGetDef(const std::string &defName) const;
    template <class T> std::unordered_map <std::string, std::shared_ptr <T>> getDefs() const;

    template <class T> void loadDefs_file(const std::string &filePath, const std::string &dataFileRootName);
    template <class T> void loadDefs_directory(const std::string &directoryPath, const std::string &dataFileRootName);

    void callOnLoadedAllDefs();
    void dropAllDefs();

private:
    static const std::string k_directoryScanDataFileExtension;

    std::unordered_map <std::string, std::shared_ptr <Def>> m_defs;
};

template <class T> void DefDatabase::DefsList <T>::expose(DataFile::Node &node)
{
    node.var(defs, "list");
}

template <class T> std::shared_ptr <T> DefDatabase::getDef(const std::string &defName) const
{
    TRACK;

    const auto &ret = tryGetDef <T> (defName);

    if(!ret)
        throw Exception{"Could not find def named \"" + defName + "\"."};

    return ret;
}

template <class T> std::shared_ptr <T> DefDatabase::tryGetDef(const std::string &defName) const
{
    TRACK;

    const auto &it = m_defs.find(defName);

    if(it == m_defs.end())
        return nullptr;

    const auto &ret = std::dynamic_pointer_cast <T> (it->second);

    return ret;
}

template <class T> std::unordered_map <std::string, std::shared_ptr <T>> DefDatabase::getDefs() const
{
    std::unordered_map <std::string, std::shared_ptr <T>> ret;

    for(const auto &it : m_defs) {
        const auto &def = std::dynamic_pointer_cast <T> (it.second);

        if(def)
            ret[it.first] = def;
    }

    return ret;
}

template <class T> void DefDatabase::loadDefs_file(const std::string &filePath, const std::string &dataFileRootName)
{
    TRACK;

    DefsList <T> defsList;

    DataFile dataFile;

    dataFile.open(filePath, true);
    dataFile.load(defsList, dataFileRootName);

    for(auto &elem : defsList.defs) {
        std::string defName = elem.getDefName(); // we need a copy

        if(m_defs.find(defName) != m_defs.end()) {
            E_ERROR("Could not add def with defName: \"%s\" because def with this defName already exists.",
                    defName.c_str());
        }
        else {
            m_defs.emplace(defName, std::make_shared <T> (std::move(elem)));
        }
    }

    E_INFO("Loaded %d defs from \"%s\".", static_cast <int> (defsList.defs.size()), filePath.c_str());
}

template <class T> void DefDatabase::loadDefs_directory(const std::string &directoryPath, const std::string &dataFileRootName)
{
    TRACK;

    QDirIterator dirIt{directoryPath.c_str(), QDirIterator::Subdirectories};

    while(dirIt.hasNext()) {
        dirIt.next();

        const auto &fileInfo = dirIt.fileInfo();

        if(fileInfo.isFile() && fileInfo.suffix().toStdString() == k_directoryScanDataFileExtension)
            loadDefs_file <T> (dirIt.filePath().toStdString(), dataFileRootName);
    }
}

} // namespace engine

#endif // ENGINE_DEF_DATABASE_HPP
