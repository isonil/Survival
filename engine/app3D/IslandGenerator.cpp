#include "IslandGenerator.hpp"

#include "../util/Random.hpp"
#include "sceneNodes/Island.hpp"
#include "Device.hpp"

#include <unordered_map>
#include <unordered_set>
#include <stack>

namespace std
{

template <> struct hash <engine::IntVec2>
{
    std::size_t operator() (const engine::IntVec2 &vec) const
    {
        return vec.x ^ (3 * vec.x + vec.y);
    }
};

} // namespace std

namespace engine
{
namespace app3D
{

IslandGenerator::IslandGenerator()
    : m_fieldsSideLength{k_maxIslandSideLength},
      m_noise{0.5f, 0.05f, 50.f, 3, 34573},
      m_noise2{0.6f, 0.005f, 1.f, 10, 468923},
      m_noise3{0.6f, 0.005f, 1.f, 10, 83452},
      m_noise4{0.5f, 0.05f, 50.f, 3, 8156}
{
    m_fields.resize(k_maxIslandSideLength * k_maxIslandSideLength);
}

std::shared_ptr <Island> IslandGenerator::generateIsland(const std::weak_ptr <Device> &device)
{
    generateFields();
    scaleFields();
    calculateFieldsDistanceToBorder();
    calculateHeightAtEachField();
    generateVerticesAndIndices();

    /*
    printf("Island:\n");

    for(int i = 0; i < m_fieldsSideLength; ++i) {
        for(int j = 0; j < m_fieldsSideLength; ++j) {
            putchar(getField(j, i).isIsland ? 'x' : ' ');
        }
        printf("\n");
    }

    printf("_____\n");*/

    return std::make_shared <Island> (m_vertices, m_indices, device);
}

IslandGenerator::Field &IslandGenerator::getField(int x, int y)
{
    auto index = y * m_fieldsSideLength + x;

    E_DASSERT(index >= 0 && index < static_cast <int> (m_fields.size()), "Index out of bounds.");

    return m_fields[index];
}

float IslandGenerator::getHeightAt(const IntVec2 &point)
{
    return m_noise.GetHeight(point.x, point.y);
}

IntVec2 IslandGenerator::pickRandomPoint()
{
    return {Random::rangeInclusive(-100000, 100000),
            Random::rangeInclusive(-100000, 100000)};
}

void IslandGenerator::floodFill(const IntVec2 &startPoint, std::unordered_set <IntVec2> &visited)
{
    std::stack <IntVec2> stack;

    stack.push(startPoint);

    while(!stack.empty()) {
        auto point = stack.top();
        stack.pop();

        if(visited.find(point) != visited.end())
            continue;

        if(getHeightAt(point) < k_minHeightForIsland)
            continue;

        if(static_cast <int> (visited.size()) > k_maxPointsPerIsland)
            continue;

        visited.insert(point);

        stack.push({point.x - 1, point.y});
        stack.push({point.x + 1, point.y});
        stack.push({point.x, point.y - 1});
        stack.push({point.x, point.y + 1});
    }
}

void IslandGenerator::calculateFieldsDistanceToBorder()
{
    for(int i = 0; i < m_fieldsSideLength; ++i) {
        for(int j = 0; j < m_fieldsSideLength; ++j) {
            if(!getField(j, i).isIsland)
                getField(j, i).wv_distToBorder[0] = 0.f;
            else {
                if(i - 1 >= 0 && j - 1 >= 0)
                    getField(j, i).wv_distToBorder[0] = std::min({getField(j, i - 1).wv_distToBorder[0] + 1.f,
                                                                  getField(j - 1, i).wv_distToBorder[0] + 1.f,
                                                                  getField(j - 1, i - 1).wv_distToBorder[0] + Math::k_sqrt2});
            }
        }
    }

    for(int i = m_fieldsSideLength - 1; i >= 0; --i) {
        for(int j = 0; j < m_fieldsSideLength; ++j) {
            if(!getField(j, i).isIsland)
                getField(j, i).wv_distToBorder[1] = 0.f;
            else {
                if(i + 1 < m_fieldsSideLength && j - 1 >= 0)
                    getField(j, i).wv_distToBorder[1] = std::min({getField(j, i + 1).wv_distToBorder[1] + 1.f,
                                                                  getField(j - 1, i).wv_distToBorder[1] + 1.f,
                                                                  getField(j - 1, i + 1).wv_distToBorder[1] + Math::k_sqrt2});
            }
        }
    }

    for(int i = 0; i < m_fieldsSideLength; ++i) {
        for(int j = m_fieldsSideLength - 1; j >= 0; --j) {
            if(!getField(j, i).isIsland)
                getField(j, i).wv_distToBorder[2] = 0.f;
            else {
                if(i - 1 >= 0 && j + 1 < m_fieldsSideLength)
                    getField(j, i).wv_distToBorder[2] = std::min({getField(j, i - 1).wv_distToBorder[2] + 1.f,
                                                                  getField(j + 1, i).wv_distToBorder[2] + 1.f,
                                                                  getField(j + 1, i - 1).wv_distToBorder[2] + Math::k_sqrt2});
            }
        }
    }

    for(int i = m_fieldsSideLength - 1; i >= 0; --i) {
        for(int j = m_fieldsSideLength - 1; j >= 0; --j) {
            if(!getField(j, i).isIsland)
                getField(j, i).wv_distToBorder[3] = 0.f;
            else {
                if(i + 1 < m_fieldsSideLength && j + 1 < m_fieldsSideLength)
                    getField(j, i).wv_distToBorder[3] = std::min({getField(j, i + 1).wv_distToBorder[3] + 1.f,
                                                                  getField(j + 1, i).wv_distToBorder[3] + 1.f,
                                                                  getField(j + 1, i + 1).wv_distToBorder[3] + Math::k_sqrt2});
            }
        }
    }

    for(int i = 0; i < m_fieldsSideLength; ++i) {
        for(int j = 0; j < m_fieldsSideLength; ++j) {
            getField(j, i).distanceToBorder = std::min({getField(j, i).wv_distToBorder[0],
                                                        getField(j, i).wv_distToBorder[1],
                                                        getField(j, i).wv_distToBorder[2],
                                                        getField(j, i).wv_distToBorder[3]});
        }
    }
}

void IslandGenerator::calculateHeightAtEachField()
{
    for(int i = 0; i < m_fieldsSideLength; ++i) {
        for(int j = 0; j < m_fieldsSideLength; ++j) {
            if(getField(j, i).isIsland) {
                if(i - 1 >= 0 && j - 1 >= 0 && i + 1 < m_fieldsSideLength && j + 1 < m_fieldsSideLength &&
                   getField(j, i - 1).isIsland && getField(j, i + 1).isIsland && getField(j - 1, i).isIsland && getField(j + 1, i).isIsland) {
                    getField(j, i).bottomHeight = getField(j, i).distanceToBorder + std::fabs(m_noise2.GetHeight(j, i));
                    getField(j, i).height += m_noise3.GetHeight(j, i);
                    getField(j, i).topAndBottomConnection = false;
                }
                else {
                    getField(j, i).height = 0.f;
                    getField(j, i).bottomHeight = 0.f; // connection between upper part and lower part
                    getField(j, i).topAndBottomConnection = true;
                }
            }
        }
    }

    // smooth

    for(int times = 0; times < 3; ++times) {
        for(int i = 0; i < m_fieldsSideLength; ++i) {
            for(int j = 0; j < m_fieldsSideLength; ++j) {
                if(getField(j, i).isIsland && !getField(j, i).topAndBottomConnection) {
                    int count = 1;
                    float sum = getField(j, i).height;

                    if(i - 1 >= 0) {
                        ++count;
                        sum += getField(j, i - 1).height;
                    }

                    if(j - 1 >= 0) {
                        ++count;
                        sum += getField(j - 1, i).height;
                    }

                    if(i + 1 < m_fieldsSideLength) {
                        ++count;
                        sum += getField(j, i + 1).height;
                    }

                    if(j + 1 < m_fieldsSideLength) {
                        ++count;
                        sum += getField(j + 1, i).height;
                    }

                    getField(j, i).height = sum / count;
                }
            }
        }
    }

    // try to raise terrain

    for(int times = 0; times < 8; ++times) {
        float threshold = 10.f;
        m_noise4.SetRandomSeed(Random::rangeInclusive(0, 100000));

        for(int i = 0; i < m_fieldsSideLength; ++i) {
            for(int j = 0; j < m_fieldsSideLength; ++j) {
                if(getField(j, i).isIsland && !getField(j, i).topAndBottomConnection) {
                    if(m_noise4.GetHeight(j, i) > threshold)
                        getField(j, i).height += 1.f;
                }
            }
        }
    }
}

void IslandGenerator::generateFields()
{
    int C1{}, C2{}, C3{}; // TODO: remove

    std::unordered_set <IntVec2> islandPoints;

    while(true) {
        if((C1 + C2 + C3) % 1000 == 0) {
            printf("%d %d %d\n", C1, C2, C3);
        }

        auto point = pickRandomPoint();

        if(getHeightAt(point) < k_minHeightForIsland) {
            ++C1;
            continue; // we need island point
        }

        islandPoints.clear();

        floodFill(point, islandPoints);

        if(static_cast <int> (islandPoints.size()) < k_minPointsPerIsland ||
           static_cast <int> (islandPoints.size()) > k_maxPointsPerIsland) {
            ++C2;
            continue; // too few or too many points
        }

        auto minmaxX = std::minmax_element(islandPoints.begin(), islandPoints.end(), [](const auto &lhs, const auto &rhs) {
            return lhs.x < rhs.x;
        });

        auto minmaxY = std::minmax_element(islandPoints.begin(), islandPoints.end(), [](const auto &lhs, const auto &rhs) {
            return lhs.y < rhs.y;
        });

        if(minmaxX.second->x - minmaxX.first->x + 3 > m_fieldsSideLength ||
           minmaxY.second->y - minmaxY.first->y + 3 > m_fieldsSideLength) {
            ++C3;
            continue; // too big side length (it has to fit in a 2 dimensional array)
        }

        IntVec2 midPoint{(minmaxX.first->x + minmaxX.second->x) / 2,
                         (minmaxY.first->y + minmaxY.second->y) / 2};

        for(const auto &elem : islandPoints) {
            int arrX = elem.x - midPoint.x + m_fieldsSideLength / 2;
            int arrY = elem.y - midPoint.y + m_fieldsSideLength / 2;

            getField(arrX, arrY).isIsland = true;
            getField(arrX, arrY).height = 0.f;
        }

        break;
    }

    printf("%d %d %d\n", C1, C2, C3);
}

void IslandGenerator::scaleFields()
{
    int newSideLength = m_fieldsSideLength * k_scaleFieldsFactor;

    std::vector <Field> newFields;

    newFields.resize(newSideLength * newSideLength);

    for(int i = 0; i < newSideLength; ++i) {
        for(int j = 0; j < newSideLength; ++j) {
            newFields[i * newSideLength + j] = getField(j / k_scaleFieldsFactor, i / k_scaleFieldsFactor);
        }
    }

    m_fields = newFields;
    m_fieldsSideLength = newSideLength;
}

void IslandGenerator::tryToMoveVerticesCloser(irr::video::S3DVertex &v1, irr::video::S3DVertex &v2)
{
    if(v1.Pos.Y - v2.Pos.Y > 0.95f) {
        v2.Pos.X = v1.Pos.X;
        v2.Pos.Z = v1.Pos.Z;
    }

    if(v2.Pos.Y - v1.Pos.Y > 0.95f) {
        v1.Pos.X = v2.Pos.X;
        v1.Pos.Z = v2.Pos.Z;
    }

    /*
    if(std::fabs(v1.Pos.Y - v2.Pos.Y) >= 0.95f) {
        float midX = (v1.Pos.X + v2.Pos.X) / 2.f;
        float midZ = (v1.Pos.Z + v2.Pos.Z) / 2.f;

        v1.Pos.X = midX;
        v1.Pos.Z = midZ;
        v2.Pos.X = midX;
        v2.Pos.Z = midZ;
    }*/
}

void IslandGenerator::generateVerticesAndIndices()
{
    m_vertices.clear();
    m_indices.clear();

    std::unordered_map <IntVec2, int> topPartPosToIndex;
    std::unordered_map <IntVec2, int> botPartPosToIndex;

    // generate vertices

    for(int i = 0; i < m_fieldsSideLength; ++i) {
        for(int j = 0; j < m_fieldsSideLength; ++j) {
            if(getField(j, i).isIsland) {
                auto vertexX = (j - m_fieldsSideLength / 2.f) * k_unitToRealPosScale;
                auto vertexZ = -(i - m_fieldsSideLength / 2.f) * k_unitToRealPosScale;

                m_vertices.push_back(irr::video::S3DVertex(vertexX, getField(j, i).height, vertexZ, 0.f, 0.f, 0.f, {255, 255, 255, 255}, j * k_unitToUVScale, i * k_unitToUVScale));
                topPartPosToIndex[IntVec2{j, i}] = m_vertices.size() - 1;

                m_vertices.push_back(irr::video::S3DVertex(vertexX, -getField(j, i).bottomHeight, vertexZ, 0.f, 0.f, 0.f, {255, 255, 0, 0}, j * k_unitToUVScale, i * k_unitToUVScale));
                botPartPosToIndex[IntVec2{j, i}] = m_vertices.size() - 1;
            }
        }
    }

    // generate indices

    for(int i = 0; i < m_fieldsSideLength; ++i) {
        for(int j = 0; j < m_fieldsSideLength; ++j) {
            // try to fill area with as many triangles of type (1) and (2) as possible

            /* (1) xx   (2)  x    (3) x    (4) xx
             *     x        xx        xx        x
             */

            if(j + 1 < m_fieldsSideLength && i + 1 < m_fieldsSideLength) {
                if(getField(j, i).isIsland && getField(j + 1, i).isIsland && getField(j, i + 1).isIsland) {
                    m_indices.push_back(topPartPosToIndex[IntVec2{j, i}]);
                    m_indices.push_back(topPartPosToIndex[IntVec2{j + 1, i}]);
                    m_indices.push_back(topPartPosToIndex[IntVec2{j, i + 1}]);

                    m_indices.push_back(botPartPosToIndex[IntVec2{j, i + 1}]);
                    m_indices.push_back(botPartPosToIndex[IntVec2{j + 1, i}]);
                    m_indices.push_back(botPartPosToIndex[IntVec2{j, i}]);
                }

                if(getField(j + 1, i).isIsland && getField(j + 1, i + 1).isIsland && getField(j, i + 1).isIsland) {
                    m_indices.push_back(topPartPosToIndex[IntVec2{j + 1, i}]);
                    m_indices.push_back(topPartPosToIndex[IntVec2{j + 1, i + 1}]);
                    m_indices.push_back(topPartPosToIndex[IntVec2{j, i + 1}]);

                    m_indices.push_back(botPartPosToIndex[IntVec2{j, i + 1}]);
                    m_indices.push_back(botPartPosToIndex[IntVec2{j + 1, i + 1}]);
                    m_indices.push_back(botPartPosToIndex[IntVec2{j + 1, i}]);
                }
            }

            // now try to add triangles of type (3) and (4)

            if(i - 1 >= 0 && j + 1 < m_fieldsSideLength) {
                if(getField(j, i).isIsland && getField(j, i - 1).isIsland && getField(j + 1, i).isIsland && !getField(j + 1, i - 1).isIsland) {
                    m_indices.push_back(topPartPosToIndex[IntVec2{j, i}]);
                    m_indices.push_back(topPartPosToIndex[IntVec2{j, i - 1}]);
                    m_indices.push_back(topPartPosToIndex[IntVec2{j + 1, i}]);

                    m_indices.push_back(botPartPosToIndex[IntVec2{j + 1, i}]);
                    m_indices.push_back(botPartPosToIndex[IntVec2{j, i - 1}]);
                    m_indices.push_back(botPartPosToIndex[IntVec2{j, i}]);
                }

                if(!getField(j, i).isIsland && getField(j, i - 1).isIsland && getField(j + 1, i - 1).isIsland && getField(j + 1, i).isIsland) {
                    m_indices.push_back(topPartPosToIndex[IntVec2{j, i - 1}]);
                    m_indices.push_back(topPartPosToIndex[IntVec2{j + 1, i - 1}]);
                    m_indices.push_back(topPartPosToIndex[IntVec2{j + 1, i}]);

                    m_indices.push_back(botPartPosToIndex[IntVec2{j + 1, i}]);
                    m_indices.push_back(botPartPosToIndex[IntVec2{j + 1, i - 1}]);
                    m_indices.push_back(botPartPosToIndex[IntVec2{j, i - 1}]);
                }
            }
        }
    }

    /*
    for(size_t i = 2; i < m_indices.size(); i += 3) {
        tryToMakeVerticesCloser(m_vertices[m_indices[i - 2]], m_vertices[m_indices[i - 1]]);
        tryToMakeVerticesCloser(m_vertices[m_indices[i - 1]], m_vertices[m_indices[i]]);
        tryToMakeVerticesCloser(m_vertices[m_indices[i]], m_vertices[m_indices[i - 2]]);
    }
    */
}

const int IslandGenerator::k_maxIslandSideLength{50};
const float IslandGenerator::k_minHeightForIsland{10.f};
const int IslandGenerator::k_minPointsPerIsland{32 * 32};
const int IslandGenerator::k_maxPointsPerIsland{49 * 49};
const float IslandGenerator::k_scaleFieldsFactor{1.f};
const float IslandGenerator::k_unitToUVScale{0.1f};
const float IslandGenerator::k_unitToRealPosScale{0.5f * 2.f * 2.f};

} // namespace app3D
} // namespace engine
