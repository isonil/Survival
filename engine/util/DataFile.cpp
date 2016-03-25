#include "DataFile.hpp"

#include "StringUtility.hpp"

#include <fstream>

namespace engine
{

void operator >> (const YAML::Node &node, bool &val)
{
    TRACK;

    val = false;

    std::string str;
    node >> str;

    for(auto &elem : str) {
        elem = std::tolower(elem);
    }

    if(str == "true" || str == "yes")
        val = true;

    if(!str.empty() && std::isdigit(str[0]) && str[0] != '0')
        val = true;
}

DataFile::Activity::Activity(Type type, const std::string &filePath)
    : m_type{type},
      m_filePath{filePath},
      m_error{}
{
}

DataFile::Activity::Type DataFile::Activity::getType() const
{
    return m_type;
}

std::string DataFile::Activity::getFilePath() const
{
    return m_filePath;
}

void DataFile::Activity::setError(bool error)
{
    m_error = error;
}

bool DataFile::Activity::getError() const
{
    return m_error;
}

DataFile::Node::Node(Activity &activity, const std::string &key, const YAML::Node *YAMLNode, bool isSequence, int sequenceElementIndex)
    : m_activity{activity},
      m_key{key},
      m_YAMLNode{YAMLNode},
      m_isSequence{isSequence},
      m_sequenceElementIndex{sequenceElementIndex}
{
}

DataFile::Activity::Type DataFile::Node::getActivityType() const
{
    return m_activity.getType();
}

std::string DataFile::Node::toString(bool onlyInnerContent) const
{
    TRACK;

    std::ostringstream oss;

    if(!onlyInnerContent)
        oss << getFormattedKey(m_key) << ':';

    if(!m_value.empty()) {
        if(!onlyInnerContent) oss << ' ';
        oss << m_value << std::endl;
    }
    else {
        if(!onlyInnerContent) oss << std::endl;

        for(const auto &elem : m_nodes) {
            std::stringstream oss2;

            if(m_isSequence)
                oss2 << elem.toString(true);
            else
                oss2 << elem.toString();

            std::string line;
            int lineNumber{};
            while(std::getline(oss2, line)) {
                if(!onlyInnerContent) oss << k_indentation;

                if(m_isSequence) {
                    if(lineNumber == 0) oss << "- ";
                    else oss << "  ";
                }

                oss << line << std::endl;
                ++lineNumber;
            }
        }
    }

    return oss.str();
}

bool DataFile::Node::isSequenceElement()
{
    return m_sequenceElementIndex >= 0;
}

const std::string DataFile::Node::k_indentation{"    "};

DataFile::DataFile()
    : m_path{"undefined"}
{
}

bool DataFile::open(const std::string &path, bool mustExist)
{
    TRACK;

    m_path = path;
    m_data = "";

    std::ifstream in{m_path};

    if(!in.is_open()) {
        if(mustExist)
            throw Exception{"Could not open DataFile file \"" + m_path + "\" (r)."};
        else
            return false;
    }

    std::ostringstream oss;

    for(std::string line; std::getline(in, line);) {
        oss << line << std::endl;
    }

    m_data = oss.str();

    return true;
}

void DataFile::save(Saveable &entry, const std::string &entryKey)
{
    TRACK;

    Activity activity{Activity::Type::Saving, m_path};
    Node node{activity, entryKey, nullptr};

    try {
        entry.expose(node);
    }
    catch(const std::exception &e) {
        throw Exception{"An exception was thrown in expose method (" + std::string{e.what()} + ") for \"" + m_path + "\"."};
    }

    if(activity.getError())
        throw Exception{"Could not save DataFile \"" + m_path + "\"."};

    m_data = node.toString();

    std::ofstream out{m_path};

    if(!out.is_open())
        throw Exception{"Could not open DataFile file \"" + m_path + "\" (rw)."};

    out << m_data;
}

void DataFile::load(Saveable &entry, const std::string &entryKey)
{
    TRACK;

    std::istringstream iss{m_data};

    if(m_data.empty())
        throw Exception{"Tried to load empty data for \"" + m_path + "\"."};

    try {
        YAML::Parser parser{iss};
        YAML::Node doc;
        parser.GetNextDocument(doc);

        const auto *YAMLNode = doc.FindValue(entryKey);

        if(!YAMLNode)
            throw Exception{"Entry key \"" + entryKey + "\" does not exist in \"" + m_path + "\"."};

        Activity activity{Activity::Type::Loading, m_path};
        Node node{activity, entryKey, YAMLNode};

        try {
            entry.expose(node);
        }
        catch(const std::exception &e) {
            throw Exception{"An exception was thrown in expose method (" + std::string{e.what()} + ") for \"" + m_path + "\"."};
        }

        if(activity.getError())
            throw Exception{"Could not load DataFile \"" + m_path + "\"."};

        Activity initActivity{Activity::Type::PostLoadInit, m_path};
        Node initNode{initActivity, entryKey, nullptr};

        try {
            entry.expose(initNode);
        }
        catch(const std::exception &e) {
            throw Exception{"An exception was thrown in expose method (" + std::string(e.what()) + ") for \"" + m_path + "\"."};
        }

        if(initActivity.getError())
            throw Exception{"Could not init all nodes in DataFile \"" + m_path + "\"."};
    }
    catch(const YAML::Exception &e) {
        throw Exception{"Could not parse DataFile file \"" + m_path + "\" (" + e.what() + ")."};
    }
}

std::string DataFile::getFormattedKey(const std::string &str)
{
    return getQuotedString(str);
}

std::string DataFile::getFormattedStrValue(const std::string &str)
{
    if(StringUtility::isPrintable(str) && !StringUtility::noNewlines(str)) {
        return getStringLiteral(str);
    }
    else {
        return getQuotedString(str);
    }
}

std::string DataFile::getQuotedString(const std::string &str)
{
    YAML::Emitter emitter;
    emitter << YAML::EmitNonAscii << str;
    return emitter.c_str();
}

std::string DataFile::getStringLiteral(const std::string &str)
{
    YAML::Emitter emitter;
    emitter << YAML::Literal << str;
    return emitter.c_str();
}

} // namespace engine
