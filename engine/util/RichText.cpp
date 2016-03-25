#include "RichText.hpp"

#include "Trace.hpp"

namespace engine
{

RichText::Part::Part(const std::string &text,
                     const Color &color,
                     Attribute attribute)
    : m_text{text}, m_color{color}, m_attribute{attribute}
{
}

const std::string &RichText::Part::getText() const
{
    return m_text;
}

const Color &RichText::Part::getColor() const
{
    return m_color;
}

RichText::Attribute RichText::Part::getAttribute() const
{
    return m_attribute;
}

RichText::RichText()
    : m_currentAttribute{Attribute::Default}
{
}

RichText &RichText::operator << (const std::string &text)
{
    TRACK;

    m_parts.emplace_back(text, m_currentColor, m_currentAttribute);
    return *this;
}

RichText &RichText::operator << (int val)
{
    TRACK;

    m_parts.emplace_back(std::to_string(val), m_currentColor, m_currentAttribute);
    return *this;
}

RichText &RichText::operator << (char ch)
{
    TRACK;

    std::string text{1, ch};
    m_parts.emplace_back(text, m_currentColor, m_currentAttribute);

    return *this;
}

RichText &RichText::operator << (const Color &color)
{
    m_currentColor = color;
    return *this;
}

RichText &RichText::operator << (Attribute attribute)
{
    m_currentAttribute = attribute;
    return *this;
}

RichText &RichText::operator << (const Part &part)
{
    TRACK;

    m_parts.push_back(part);
    m_currentColor = m_parts.back().getColor();
    m_currentAttribute = m_parts.back().getAttribute();

    return *this;
}

RichText &RichText::operator << (const RichText &text)
{
    TRACK;

    const auto &parts = text.getParts();

    for(const auto &elem : parts) {
        *this << elem;
    }

    if(!m_parts.empty()) {
        m_currentColor = m_parts.back().getColor();
        m_currentAttribute = m_parts.back().getAttribute();
    }

    return *this;
}

const std::vector <RichText::Part> &RichText::getParts() const
{
    return m_parts;
}

bool RichText::isEmpty() const
{
    return !std::any_of(m_parts.begin(), m_parts.end(), [](const auto &elem) {
        return !elem.getText().empty();
    });
}

std::string RichText::toString() const
{
    TRACK;

    std::ostringstream text;

    for(const auto &elem : m_parts) {
        text << elem.getText();
    }

    return text.str();
}

void RichText::clear()
{
    m_parts.clear();
    m_currentColor = Color();
    m_currentAttribute = Attribute::Default;
}

} // namespace engine
