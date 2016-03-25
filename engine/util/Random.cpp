#include "Random.hpp"

#include <ctime>
#include <limits>

namespace engine
{

int Random::nextInt()
{
    return m_randIntDistribution(m_randGenerator);
}

int Random::nextInt(int seed)
{
    m_customRandGenerator.seed(seed);
    return m_randIntDistribution(m_customRandGenerator);
}

int Random::nextInt(Generator &generator)
{
    return m_randIntDistribution(generator);
}

float Random::nextFloat()
{
    return m_randFloatDistribution(m_randGenerator);
}

float Random::nextFloat(int seed)
{
    m_customRandGenerator.seed(seed);
    return m_randFloatDistribution(m_customRandGenerator);
}

float Random::nextFloat(Generator &generator)
{
    return m_randFloatDistribution(generator);
}

float Random::nextGaussian()
{
    return m_randNormalDistribution(m_randGenerator);
}

float Random::nextGaussian(int seed)
{
    m_customRandGenerator.seed(seed);
    return m_randNormalDistribution(m_customRandGenerator);
}

float Random::nextGaussian(Generator &generator)
{
    return m_randNormalDistribution(generator);
}

int Random::rangeInclusive(int start, int end)
{
    if(start > end) return start;
    return start + nextInt() % (end - start + 1); // TODO: it's not uniformly distributed
}

int Random::rangeInclusive(int start, int end, int seed)
{
    if(start > end) return start;
    return start + nextInt(seed) % (end - start + 1); // TODO: it's not uniformly distributed
}

int Random::rangeInclusive(int start, int end, Generator &generator)
{
    if(start > end) return start;
    return start + nextInt(generator) % (end - start + 1); // TODO: it's not uniformly distributed
}

float Random::rangeInclusive(float start, float end)
{
    // for floats rangeInclusive and rangeExclusive are the same
    return rangeExclusive(start, end);
}

float Random::rangeInclusive(float start, float end, int seed)
{
    // for floats rangeInclusive and rangeExclusive are the same
    return rangeExclusive(start, end, seed);
}

float Random::rangeInclusive(float start, float end, Generator &generator)
{
    // for floats rangeInclusive and rangeExclusive are the same
    return rangeExclusive(start, end, generator);
}

int Random::rangeExclusive(int start, int end)
{
    if(start >= end) return start;
    return start + nextInt() % (end - start); // TODO: it's not uniformly distributed
}

int Random::rangeExclusive(int start, int end, int seed)
{
    if(start >= end) return start;
    return start + nextInt(seed) % (end - start); // TODO: it's not uniformly distributed
}

int Random::rangeExclusive(int start, int end, Generator &generator)
{
    if(start >= end) return start;
    return start + nextInt(generator) % (end - start); // TODO: it's not uniformly distributed
}

float Random::rangeExclusive(float start, float end)
{
    if(start >= end) return start;
    return start + nextFloat() * (end - start);
}

float Random::rangeExclusive(float start, float end, int seed)
{
    if(start >= end) return start;
    return start + nextFloat(seed) * (end - start);
}

float Random::rangeExclusive(float start, float end, Generator &generator)
{
    if(start >= end) return start;
    return start + nextFloat(generator) * (end - start);
}

const int Random::k_randRange{std::numeric_limits <int>::max() - 1};
std::uniform_int_distribution <int> Random::m_randIntDistribution{0, k_randRange};
std::uniform_real_distribution <float> Random::m_randFloatDistribution{0.f, 1.f};
std::normal_distribution <float> Random::m_randNormalDistribution{0.f, 1.f};
std::mt19937 Random::m_randGenerator(std::time(nullptr));
std::mt19937 Random::m_customRandGenerator{};

} // namespace engine
