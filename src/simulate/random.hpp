#pragma once
#include <random>
#include "type.hpp"

namespace sanity::simulate
{
class UniformSampler
{
    std::mt19937_64 gen;
    std::uniform_real_distribution<Real> dis;

public:
    UniformSampler() : gen(std::random_device()()), dis(0.0, 1.0) {}
    UniformSampler(uint seed) : gen(seed), dis(0.0, 1.0) {}
    UniformSampler(uint seed, Real a, Real b) : gen(seed), dis(a, b) {}
    Real operator()() { return dis(gen); }
};
}  // namespace sanity::simulate
