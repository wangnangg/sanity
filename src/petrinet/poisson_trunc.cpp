#include "poisson_trunc.hpp"
#include <tuple>
#include <vector>

namespace sanity::petrinet
{
std::tuple<uint, uint, std::vector<Real>> poissonTruncPoint(Real lambda,
                                                            Real tol)
{
    Real k = 0.0;
    Real expo = -lambda;
    Real term = exp(expo);
    Real term_sum = term;
    Real log_lmd = std::log(lambda);
    auto probs = std::vector<Real>();
    while (term_sum < tol / 2.0)
    {
        k += 1.0;
        expo += log_lmd - log(k);
        term = exp(expo);
        term_sum += term;
    }

    uint ltp = (uint)k;
    probs.push_back(term);

    while (1.0 - term_sum > tol / 2.0)
    {
        k += 1.0;
        expo += log_lmd - std::log(k);
        term = exp(expo);
        term_sum += term;
        probs.push_back(term);
    }

    uint rtp = (uint)k;

    return std::make_tuple(ltp, rtp, probs);
}
}  // namespace sanity::petrinet
