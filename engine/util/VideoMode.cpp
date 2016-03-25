#include "VideoMode.hpp"

#include "Exception.hpp"

namespace engine
{

VideoMode::VideoMode(int width, int height)
    : m_width{width}, m_height{height}
{
    validate();
}

void VideoMode::expose(DataFile::Node &node)
{
    node.var(m_width, "width");
    node.var(m_height, "height");

    if(node.getActivityType() == DataFile::Activity::Type::PostLoadInit)
        validate();
}

int VideoMode::getWidth() const
{
    return m_width;
}

int VideoMode::getHeight() const
{
    return m_height;
}

void VideoMode::validate() const
{
    if(m_width <= 0 || m_height <= 0)
        throw Exception{"Invalid VideoMode resolution (" + std::to_string(m_width) + 'x' + std::to_string(m_height) + ")."};
}

} // namespace engine
