#include "WorldPartTopographyInfo.hpp"

#include "engine/app3D/sceneNodes/Terrain.hpp"
#include "engine/app3D/defs/TerrainDef.hpp"

namespace app
{

WorldPartTopographyInfo::WorldPartTopographyInfo(const engine::app3D::Terrain &terrain)
    : m_size{}
{
    TRACK;

    auto size = static_cast <int> (terrain.getDef().getScale() / k_fieldSize);

    m_size = size;
    m_fields.reserve(m_size * m_size);

    for(int y = 0; y < size; ++y) {
        for(int x = 0; x < size; ++x) {
            float realX{x * k_fieldSize + k_fieldSize / 2.f};
            float realY{y * k_fieldSize + k_fieldSize / 2.f};

            auto height = terrain.getHeight({realX, realY});

            m_fields.emplace_back();
            m_fields.back().height = height;
            m_fields.back().pos = {realX, height, realY};
        }
    }

    calculateSlope();
    calculateDistanceToWater();
}

float WorldPartTopographyInfo::getSlope(engine::FloatVec2 pos)
{
    TRACK;

    posToBounds(pos);

    calculateClosestFields(pos);

    if(m_closestFields.empty())
        return 0.f;
    else if(m_closestFields.size() == 1)
        return m_closestFields[0].slope;
    else if(m_closestFields.size() == 2) {
        if(engine::Math::fuzzyCompare(m_closestFields[0].pos.x, m_closestFields[1].pos.x)) {
            float diff{std::fabs(m_closestFields[0].pos.z - m_closestFields[1].pos.z)};
            float factor1{std::fabs(pos.y - m_closestFields[0].pos.z) / diff};
            float factor2{1.f - factor1};

            return m_closestFields[1].slope * factor1 + m_closestFields[0].slope * factor2;
        }
        else {
            float diff{std::fabs(m_closestFields[0].pos.x - m_closestFields[1].pos.x)};
            float factor1{std::fabs(pos.x - m_closestFields[0].pos.x) / diff};
            float factor2{1.f - factor1};

            return m_closestFields[1].slope * factor1 + m_closestFields[0].slope * factor2;
        }
    }
    else {
        E_DASSERT(m_closestFields.size() == 4, "Impossible.");

        /* 0-----1
         * |     |
         * |     |
         * 2-----3
         */

        float xDiff{std::fabs(m_closestFields[0].pos.x - m_closestFields[1].pos.x)};
        float zDiff{std::fabs(m_closestFields[0].pos.z - m_closestFields[2].pos.z)};

        float interpolatedTop{std::fabs(pos.x - m_closestFields[0].pos.x) / xDiff * m_closestFields[1].slope + std::fabs(pos.x - m_closestFields[1].pos.x) / xDiff * m_closestFields[0].slope};
        float interpolatedBottom{std::fabs(pos.x - m_closestFields[2].pos.x) / xDiff * m_closestFields[3].slope + std::fabs(pos.x - m_closestFields[3].pos.x) / xDiff * m_closestFields[2].slope};
        float interpolated{std::fabs(pos.y - m_closestFields[0].pos.z) / zDiff * interpolatedBottom + std::fabs(pos.y - m_closestFields[2].pos.z) / zDiff * interpolatedTop};

        return interpolated;
    }
}

float WorldPartTopographyInfo::getDistanceToWater(engine::FloatVec2 pos)
{
    // TODO
    return 0.f;
}

WorldPartTopographyInfo::Field &WorldPartTopographyInfo::getField(int x, int y)
{
    E_DASSERT(x >= 0 && x < m_size && y >= 0 && y < m_size, "Index out of bounds.");

    return m_fields[y * m_size + x];
}

const WorldPartTopographyInfo::Field &WorldPartTopographyInfo::getField(int x, int y) const
{
    E_DASSERT(x >= 0 && x < m_size && y >= 0 && y < m_size, "Index out of bounds.");

    return m_fields[y * m_size + x];
}

bool WorldPartTopographyInfo::isInBounds(int x, int y) const
{
    return x >= 0 && x < m_size && y >= 0 && y < m_size;
}

void WorldPartTopographyInfo::calculateSlope()
{
    TRACK;

    for(int y = 0; y < m_size; ++y) {
        for(int x = 0; x < m_size; ++x) {
            auto &field = getField(x, y);
            auto height = field.height;

            float diffUp{};
            float diffDown{};
            float diffLeft{};
            float diffRight{};

            if(isInBounds(x, y - 2))
                diffUp = fabs(height - getField(x, y - 2).height);

            if(isInBounds(x, y + 2))
                diffDown = fabs(height - getField(x, y + 2).height);

            if(isInBounds(x - 2, y))
                diffLeft = fabs(height - getField(x - 2, y).height);

            if(isInBounds(x + 2, y))
                diffRight = fabs(height - getField(x + 2, y).height);

            float maxDiff{std::max({diffUp, diffDown, diffLeft, diffRight})};

            field.slope = atan(maxDiff / (2.f * k_fieldSize)) / (0.5f * engine::Math::k_pi);
        }
    }
}

void WorldPartTopographyInfo::calculateDistanceToWater()
{
    // TODO
}

void WorldPartTopographyInfo::posToBounds(engine::FloatVec2 &pos) const
{
    pos.x = engine::Math::clamp(pos.x, 0.f, m_size * k_fieldSize);
    pos.y = engine::Math::clamp(pos.y, 0.f, m_size * k_fieldSize);
}

void WorldPartTopographyInfo::calculateClosestFields(engine::FloatVec2 pos)
{
    TRACK;

    m_closestFields.clear();

    if(!m_size)
        return;

    posToBounds(pos);

    auto x = engine::Math::approxToInt((pos.x - k_fieldSize / 2.f) / k_fieldSize);
    auto y = engine::Math::approxToInt((pos.y - k_fieldSize / 2.f) / k_fieldSize);

    x = engine::Math::clamp(x, 0, m_size - 1);
    y = engine::Math::clamp(y, 0, m_size - 1);

    m_closestFields.push_back(getField(x, y));

    if(engine::Math::fuzzyCompare(pos.x, m_closestFields.back().pos.x) &&
       engine::Math::fuzzyCompare(pos.y, m_closestFields.back().pos.z)) // 2d vs 3d
        return; // if we query for center, then there is no need for further interpolation

    if(isInBounds(x + 1, y))
        m_closestFields.push_back(getField(x + 1, y));

    if(isInBounds(x, y + 1))
        m_closestFields.push_back(getField(x, y + 1));

    if(isInBounds(x + 1, y + 1))
        m_closestFields.push_back(getField(x + 1, y + 1));
}

const float WorldPartTopographyInfo::k_fieldSize{1.f};

} // namespace app
