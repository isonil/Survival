#ifndef ENGINE_RICH_TEXT_HPP
#define ENGINE_RICH_TEXT_HPP

#include "Color.hpp"

#include <string>
#include <vector>

namespace engine
{

class RichText
{
public:
    enum class Attribute
    {
        Default,
        Bold,
        Italic,
        BoldItalic
    };

    class Part
    {
    public:
        explicit Part(const std::string &text,
                      const Color &color = {},
                      Attribute attribute = Attribute::Default);

        const std::string &getText() const;
        const Color &getColor() const;
        Attribute getAttribute() const;

    private:
        std::string m_text;
        Color m_color;
        Attribute m_attribute;
    };

    RichText();

    RichText &operator << (const std::string &text);
    RichText &operator << (int val);
    RichText &operator << (char ch);
    RichText &operator << (const Color &color);
    RichText &operator << (Attribute attribute);
    RichText &operator << (const Part &part);
    RichText &operator << (const RichText &text);

    const std::vector <Part> &getParts() const;
    bool isEmpty() const;
    std::string toString() const;
    void clear();

private:
    std::vector <Part> m_parts;
    Color m_currentColor;
    Attribute m_currentAttribute;
};

} // namespace engine

#endif // ENGINE_RICH_TEXT_HPP
