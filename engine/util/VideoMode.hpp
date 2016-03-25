#ifndef ENGINE_VIDEO_MODE_HPP
#define ENGINE_VIDEO_MODE_HPP

#include "DataFile.hpp"

namespace engine
{

class VideoMode : DataFile::Saveable
{
public:
    VideoMode(int width, int height);

    void expose(DataFile::Node &node) override;

    int getWidth() const;
    int getHeight() const;

private:
    void validate() const;

    int m_width, m_height;
};

} // namespace engine

#endif // ENGINE_VIDEO_MODE_HPP
