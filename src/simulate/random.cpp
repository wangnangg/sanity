#include "random.hpp"
#include <cassert>
#include <cmath>

namespace sanity::simulate
{
Real mean(const std::vector<Real>& samples)
{
    Real sum = 0;
    for (Real v : samples)
    {
        sum += v;
    }
    return sum / samples.size();
}

Real variance(const std::vector<Real>& samples, Real mean)
{
    Real s2 = 0;
    for (Real v : samples)
    {
        s2 += (v - mean) * (v - mean);
    }
    return s2 / samples.size();
}

Real sampleVariance(const std::vector<Real>& samples, Real mean)
{
    Real s2 = 0;
    for (Real v : samples)
    {
        s2 += (v - mean) * (v - mean);
    }
    return s2 / (samples.size() - 1);
}

Real stdNormDistCdf(Real x)
{
    Real mu = 0.0;
    Real sigma = 1.0;
    return 0.5 * (1.0 + std::erf((x - mu) / (std::sqrt(2.0) * sigma)));
}
Real stdNormDistQuantile(Real p)
{
    Real err = 1e-5;
    assert(p > err);
    assert(p < 1.0 - err);
    Real x = 0;
    Real step = 1.0;
    Real prob = stdNormDistCdf(x);
    if (std::abs(prob - p) <= err)
    {
        return x;
    }
    Real right, left;
    if (prob > p)
    {
        right = x;
        do
        {
            x -= step;
            prob = stdNormDistCdf(x);
        } while (prob > p);
        if (std::abs(prob - p) <= err)
        {
            return x;
        }
        left = x;
    }
    else
    {
        left = x;
        do
        {
            x += step;
            prob = stdNormDistCdf(x);
        } while (prob < p);
        if (std::abs(prob - p) <= err)
        {
            return x;
        }
        right = x;
    }

    do
    {
        x = (left + right) / 2.0;
        prob = stdNormDistCdf(x);
        if (prob > p)
        {
            right = x;
        }
        else
        {
            left = x;
        }
    } while (std::abs(prob - p) > err);

    return x;
}

Interval confidenceInterval(const std::vector<Real>& samples, Real confidence)
{
    if (samples.size() < 10)
    {
        throw std::invalid_argument(
            "You need at least 10 samples to compute confidence interval.");
    }
    Real mu = mean(samples);
    Real var = sampleVariance(samples, mu);
    Real s = std::sqrt(var);
    Real n = samples.size();

    Real left_p = stdNormDistQuantile((1.0 - confidence) / 2.0);
    Real half = (-left_p) * s / std::sqrt(n);

    Interval itv;
    itv.begin = mu - half;
    itv.end = mu + half;

    return itv;
}
}  // namespace sanity::simulate
