#ifndef ENGINE_D_HONDT_DISTRIBUTION_HPP
#define ENGINE_D_HONDT_DISTRIBUTION_HPP

#include <vector>
#include <algorithm>

namespace engine
{

class DHondtDistribution
{
public:
    template <typename Iterator, typename DensityGetter>
    static std::vector <int> distribute(const Iterator &beg, const Iterator &end, const DensityGetter &getter, int toDistribute);
};

template <typename Iterator, typename DensityGetter>
std::vector <int> DHondtDistribution::distribute(const Iterator &beg, const Iterator &end, const DensityGetter &getter, int toDistribute)
{
    if(beg == end)
        return {};

    auto count = std::distance(beg, end);

    std::vector <int> ret(count);

    std::vector <double> density;
    density.reserve(count);

    for(auto it = beg; it != end; ++it) {
        density.emplace_back(getter(*it));
    }

    std::vector <double> calcVec = density;

    for(int i = 0; i < toDistribute; ++i) {
        auto index = std::distance(calcVec.begin(), std::max_element(calcVec.begin(), calcVec.end()));
        ++ret[index];
        calcVec[index] = density[index] / (ret[index] + 1.0);
    }

    return ret;
}

} // namespace engine

#endif // ENGINE_D_HONDT_DISTRIBUTION_HPP
