#pragma once
#include <random>
#include <vector>
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

struct Interval
{
    Real begin;
    Real end;
    Real center() const { return (begin + end) / 2.0; }
};

Real mean(const std::vector<Real>& samples);
Real variance(const std::vector<Real>& samples, Real mean);
Real sampleVariance(const std::vector<Real>& samples, Real mean);

Real stdNormDistCdf(Real x);
Real stdNormDistQuantile(Real quantile);

Interval confidenceInterval(const std::vector<Real>& samples,
                            Real confidence);

}  // namespace sanity::simulate
