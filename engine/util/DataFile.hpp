#ifndef ENGINE_DATA_FILE_HPP
#define ENGINE_DATA_FILE_HPP

#include "../ext/optional.hpp"
#include "LogManager.hpp"
#include "Exception.hpp"
#include "Trace.hpp"

#include <yaml-cpp/yaml.h>

#include <cctype>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace engine
{

void operator >> (const YAML::Node &node, bool &val);

class DataFile : public Tracked <DataFile>
{
public:
    class Node;

    class Saveable
    {
    public:
        virtual void expose(Node &) = 0;

        virtual ~Saveable() = default;
    };

    class Activity
    {
    public:
        enum class Type
        {
            Saving,
            Loading,
            PostLoadInit
        };

        Activity(Type type, const std::string &filePath);
        Activity(const Activity &) = default;

        Type getType() const;
        std::string getFilePath() const;
        void setError(bool error);
        bool getError() const;

    private:
        Type m_type;
        std::string m_filePath;
        bool m_error;
    };

    class Node
    {
    public:
        Node(Activity &activity, const std::string &key, const YAML::Node *YAMLNode, bool isSequence = {}, int sequenceElementIndex = -1);

        // 'var' version for Saveable instances
        template <typename T, typename std::enable_if <std::is_base_of <Saveable, T>::value, T>::type * = nullptr>
        void var(T &v, const std::string &key);

        // 'var' version for Saveable instances with default value
        template <typename T, typename std::enable_if <std::is_base_of <Saveable, T>::value, T>::type * = nullptr>
        void var(T &v, const std::string &key, const typename std::common_type <T>::type &defaultValue);

        // 'var' version for non-Saveable instances
        template <typename T, typename std::enable_if <!std::is_base_of <Saveable, T>::value, T>::type * = nullptr>
        void var(T &v, const std::string &key);

        // 'var' version for non-Saveable instances with default value
        // we use std::common_type, so type is deduced only from the first argument,
        // so var(stdString, "foo", "default"); works (otherwise T couldn't be deduced)
        template <typename T, typename std::enable_if <!std::is_base_of <Saveable, T>::value, T>::type * = nullptr>
        void var(T &v, const std::string &key, const typename std::common_type <T>::type &defaultValue);

        // 'var' version for std::vector
        template <typename T> void var(std::vector <T> &v, const std::string &key);

        // 'var' version for std::map
        template <typename T1, typename T2> void var(std::map <T1, T2> &v, const std::string &key);

        Activity::Type getActivityType() const;
        std::string toString(bool onlyInnerContent = {}) const;

    private:
        bool isSequenceElement();

        // 'var' version for non-Saveable instances with or without default value
        template <typename T, typename std::enable_if <!std::is_base_of <Saveable, T>::value, T>::type * = nullptr>
        void var_nonSaveableWithOptional(T &v, const std::string &key, const std::experimental::optional <T> &defaultValue);

        static const std::string k_indentation;

        Activity &m_activity;
        std::string m_key;
        std::vector <Node> m_nodes;
        const YAML::Node *m_YAMLNode;
        bool m_isSequence;
        int m_sequenceElementIndex;
        std::string m_value;
    };

    DataFile();
    DataFile(const DataFile &) = delete;

    DataFile &operator = (const DataFile &) = delete;

    bool open(const std::string &path, bool mustExist);
    void save(Saveable &entry, const std::string &entryKey);
    void load(Saveable &entry, const std::string &entryKey);

private:
    static std::string getFormattedKey(const std::string &str);
    static std::string getFormattedStrValue(const std::string &str);
    static std::string getQuotedString(const std::string &str);
    static std::string getStringLiteral(const std::string &str);

    std::string m_path;
    std::string m_data;
};

template <typename T, typename std::enable_if <std::is_base_of <DataFile::Saveable, T>::value, T>::type *>
void DataFile::Node::var(T &v, const std::string &key)
{
    TRACK;

    auto activityType = m_activity.getType();

    if(activityType == Activity::Type::Saving || activityType == Activity::Type::PostLoadInit) {
        m_nodes.emplace_back(m_activity, key, nullptr);

        try {
            v.expose(m_nodes.back());
        }
        catch(const std::exception &e) {
            E_ERROR("An exception was thrown in expose method for \"%s\" node (in \"%s\") (%s).",
                    key.c_str(), m_key.c_str(), e.what());

            m_activity.setError(true);
        }
    }
    else if(activityType == Activity::Type::Loading) {
        E_DASSERT(m_YAMLNode, "m_YAMLNode is nullptr.");

        try {
            const auto &node = isSequenceElement() ? (*m_YAMLNode)[m_sequenceElementIndex] : (*m_YAMLNode)[key];

            if(node.Type() != YAML::NodeType::Map && node.Type() != YAML::NodeType::Null) {
                const auto &filePath = m_activity.getFilePath();

                E_ERROR("Key \"%s\" in \"%s\" node in DataFile file \"%s\" is not a map (tried to read as Saveable).",
                        key.c_str(), m_key.c_str(), filePath.c_str());

                m_activity.setError(true);
                return;
            }

            m_nodes.emplace_back(m_activity, key, &node);

            v.expose(m_nodes.back());
        }
        catch(const YAML::Exception &e) {
            const auto &filePath = m_activity.getFilePath();

            E_ERROR("Could not read \"%s\" key in \"%s\" node in DataFile file \"%s\" (%s).",
                    key.c_str(), m_key.c_str(), filePath.c_str(), e.what());

            m_activity.setError(true);
        }
    }
}

template <typename T, typename std::enable_if <std::is_base_of <DataFile::Saveable, T>::value, T>::type *>
void DataFile::Node::var(T &v, const std::string &key, const typename std::common_type <T>::type &defaultValue)
{
    TRACK;

    // HACK: this method is almost the same as 'var' for Saveable without default value;
    // there can't be one common base for these two methods using std::optional, because
    // 'var' version with default value requires T to be copyable
    // the same applies to non-Saveables, but there is common base currently
    // (so all non-Saveables must be copyable currently)

    auto activityType = m_activity.getType();

    if(activityType == Activity::Type::Saving || activityType == Activity::Type::PostLoadInit) {
        m_nodes.emplace_back(m_activity, key, nullptr);

        try {
            v.expose(m_nodes.back());
        }
        catch(const std::exception &e) {
            E_ERROR("An exception was thrown in expose method for \"%s\" node (in \"%s\") (%s).",
                    key.c_str(), m_key.c_str(), e.what());

            m_activity.setError(true);
        }
    }
    else if(activityType == Activity::Type::Loading) {
        E_DASSERT(m_YAMLNode, "m_YAMLNode is nullptr.");

        try {
            const auto &node = isSequenceElement() ? (*m_YAMLNode)[m_sequenceElementIndex] : (*m_YAMLNode)[key];

            if(node.Type() != YAML::NodeType::Map && node.Type() != YAML::NodeType::Null) {
                const auto &filePath = m_activity.getFilePath();

                E_ERROR("Key \"%s\" in \"%s\" node in DataFile file \"%s\" is not a map (tried to read as Saveable).",
                        key.c_str(), m_key.c_str(), filePath.c_str());

                m_activity.setError(true);
                return;
            }

            m_nodes.emplace_back(m_activity, key, &node);

            v.expose(m_nodes.back());
        }
        catch(const YAML::Exception &e) {
            v = defaultValue;
        }
    }
}

template <typename T, typename std::enable_if <!std::is_base_of <DataFile::Saveable, T>::value, T>::type *>
void DataFile::Node::var(T &v, const std::string &key)
{
    var_nonSaveableWithOptional(v, key, std::experimental::optional <T> {});
}

template <typename T, typename std::enable_if <!std::is_base_of <DataFile::Saveable, T>::value, T>::type *>
void DataFile::Node::var(T &v, const std::string &key, const typename std::common_type <T>::type &defaultValue)
{
    var_nonSaveableWithOptional(v, key, std::experimental::optional <T> {defaultValue});
}

template <typename T> void DataFile::Node::var(std::vector <T> &v, const std::string &key)
{
    TRACK;

    auto activityType = m_activity.getType();

    if(activityType == Activity::Type::Saving || activityType == Activity::Type::PostLoadInit) {
        m_nodes.emplace_back(m_activity, key, nullptr, true);

        for(auto &elem : v) {
            m_nodes.back().var(elem, "-");
        }
    }
    else if(activityType == Activity::Type::Loading) {
        E_DASSERT(m_YAMLNode, "m_YAMLNode is nullptr.");

        v.clear();

        try {
            const auto &node = isSequenceElement() ? (*m_YAMLNode)[m_sequenceElementIndex] : (*m_YAMLNode)[key];

            if(node.Type() != YAML::NodeType::Sequence && node.Type() != YAML::NodeType::Null) {
                const auto &filePath = m_activity.getFilePath();

                E_ERROR("Key \"%s\" in \"%s\" node in DataFile file \"%s\" is not a sequence (tried to read as std::vector).",
                        key.c_str(), m_key.c_str(), filePath.c_str());

                m_activity.setError(true);
                return;
            }

            v.reserve(node.size());

            for(size_t i = 0; i < node.size(); ++i) {
                T tmp{};
                m_nodes.emplace_back(m_activity, "-", &node, false, i);
                m_nodes.back().var(tmp, "-");
                v.emplace_back(std::move(tmp));
            }
        }
        catch(const YAML::Exception &e) {
            // vector is always optional
        }
    }
}

template <typename T1, typename T2> void DataFile::Node::var(std::map <T1, T2> &v, const std::string &key)
{
    TRACK;

    auto activityType = m_activity.getType();

    if(activityType == Activity::Type::Saving || activityType == Activity::Type::PostLoadInit) {
        m_nodes.emplace_back(m_activity, key, nullptr);

        for(auto &elem : v) {
            std::ostringstream oss;
            oss << elem.first;
            m_nodes.back().var(elem.second, oss.str());
        }
    }
    else if(activityType == Activity::Type::Loading) {
        E_DASSERT(m_YAMLNode, "m_YAMLNode is nullptr.");

        v.clear();

        try {
            const auto &node = isSequenceElement() ? (*m_YAMLNode)[m_sequenceElementIndex] : (*m_YAMLNode)[key];

            if(node.Type() != YAML::NodeType::Map && node.Type() != YAML::NodeType::Null) {
                const auto &filePath = m_activity.getFilePath();

                E_ERROR("Key \"%s\" in \"%s\" node in DataFile file \"%s\" is not a map (tried to read as std::map).",
                        key.c_str(), m_key.c_str(), filePath.c_str());

                m_activity.setError(true);
                return;
            }

            for(const auto &elem : node) {
                T1 elementKey{};
                elem >> elementKey;

                std::ostringstream oss;
                oss << elementKey;

                const auto &elementKeyStr = oss.str();

                T2 tmp{};
                m_nodes.emplace_back(m_activity, elementKeyStr, &node);
                m_nodes.back().var(tmp, elementKeyStr);
                v.emplace(elementKey, std::move(tmp));
            }
        }
        catch(const YAML::Exception &e) {
            // map is always optional
        }
    }
}

template <typename T, typename std::enable_if <!std::is_base_of <DataFile::Saveable, T>::value, T>::type *>
void DataFile::Node::var_nonSaveableWithOptional(T &v, const std::string &key, const std::experimental::optional <T> &defaultValue)
{
    TRACK;

    auto activityType = m_activity.getType();

    if(activityType == Activity::Type::Saving) {
        m_nodes.emplace_back(m_activity, key, nullptr);

        std::ostringstream oss;
        oss << v;

        m_nodes.back().m_value = getFormattedStrValue(oss.str());
    }
    else if(activityType == Activity::Type::Loading) {
        E_DASSERT(m_YAMLNode, "m_YAMLNode is nullptr.");

        v = T{};

        try {
            const auto &node = isSequenceElement() ? (*m_YAMLNode)[m_sequenceElementIndex] : (*m_YAMLNode)[key];

            node >> v;
        }
        catch(const YAML::Exception &e) {
            if(defaultValue)
                v = *defaultValue;
            else {
                const auto &filePath = m_activity.getFilePath();

                E_ERROR("Could not read \"%s\" key in \"%s\" node in DataFile file \"%s\" (%s).",
                        key.c_str(), m_key.c_str(), filePath.c_str(), e.what());

                m_activity.setError(true);
            }
        }
    }
}

} // namespace engine

#endif // ENGINE_DATA_FILE_HPP
