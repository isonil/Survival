#ifndef ENGINE_RANDOM_HPP
#define ENGINE_RANDOM_HPP

#include "Exception.hpp"

#include <random>

namespace engine
{

class Random
{
public:
    using Generator = std::mt19937;

    static int nextInt();
    static int nextInt(int seed);
    static int nextInt(Generator &generator);
    static float nextFloat();
    static float nextFloat(int seed);
    static float nextFloat(Generator &generator);
    static float nextGaussian();
    static float nextGaussian(int seed);
    static float nextGaussian(Generator &generator);
    static int rangeInclusive(int start, int end);
    static int rangeInclusive(int start, int end, int seed);
    static int rangeInclusive(int start, int end, Generator &generator);
    static float rangeInclusive(float start, float end);
    static float rangeInclusive(float start, float end, int seed);
    static float rangeInclusive(float start, float end, Generator &generator);
    static int rangeExclusive(int start, int end);
    static int rangeExclusive(int start, int end, int seed);
    static int rangeExclusive(int start, int end, Generator &generator);
    static float rangeExclusive(float start, float end);
    static float rangeExclusive(float start, float end, int seed);
    static float rangeExclusive(float start, float end, Generator &generator);
    template <typename Container, typename WeightGetter> static const auto &randomElementByWeight(const Container &container, const WeightGetter &weightGetter);

private:
    static const int k_randRange;

    static std::uniform_int_distribution <int> m_randIntDistribution;
    static std::uniform_real_distribution <float> m_randFloatDistribution;
    static std::normal_distribution <float> m_randNormalDistribution;
    static std::mt19937 m_randGenerator;
    static std::mt19937 m_customRandGenerator;
};

template <typename Container, typename WeightGetter> const auto &Random::randomElementByWeight(const Container &container, const WeightGetter &weightGetter)
{
    if(container.empty())
        throw Exception{"Tried to get random element from empty container."};

    float totalWeight{std::accumulate(std::begin(container), std::end(container), 0.f, [&weightGetter](float sum, const auto &elem) {
        return sum + weightGetter(elem);
    })};

    float r{rangeExclusive(0.f, totalWeight)};
    float currSum{};

    for(const auto &elem : container) {
        currSum += weightGetter(elem);

        if(r < currSum)
            return elem;
    }

    return container.back();
}

} // namespace engine

#endif // ENGINE_RANDOM_HPP
