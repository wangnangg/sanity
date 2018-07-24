#include "srntsolve.hpp"
#include <cassert>
#include "linear.hpp"
#include "poisson_trunc.hpp"
#include "splinear.hpp"

using namespace sanity::splinear;
using namespace sanity::linear;

namespace sanity::petrinet
{
Real maxOutRate(const graph::DiGraph& rg, const std::vector<Real>& edge_rates)
{
    Real max = 0.0;
    for (uint src_idx = 0; src_idx < rg.nodeCount(); src_idx++)
    {
        Real total_rate = 0.0;
        auto src = rg.getNode(src_idx);
        for (auto e : src.edges)
        {
            total_rate += edge_rates[e.eid];
        }
        if (total_rate > max)
        {
            max = total_rate;
        }
    }
    return max;
}

Spmatrix probMatrix(const graph::DiGraph& rg,
                    const std::vector<Real>& edge_rates, Real unif_rate)
{
    auto n = rg.nodeCount();
    SpmatrixCreator crt(n, n);
    for (uint src_idx = 0; src_idx < rg.nodeCount(); src_idx++)
    {
        auto src = rg.getNode(src_idx);
        Real total_rate = 0.0;
        for (auto e : src.edges)
        {
            auto rate = edge_rates[e.eid];
            crt.addEntry(e.dst, src_idx, rate / unif_rate);
            total_rate += rate;
        }
        auto self_prob = 1.0 - total_rate / unif_rate;
        assert(self_prob >= 0.0);
        crt.addEntry(src_idx, src_idx, self_prob);
    }
    return crt.create(Spmatrix::RowCompressed);
}

linear::Vector srnTransientProb(
    const graph::DiGraph& rg, const std::vector<Real>& edge_rates,
    const std::vector<MarkingInitProb>& init_probs, Real time,
    Real unif_rate_factor, Real tol, uint ss_check_interval)
{
    auto unif_rate = maxOutRate(rg, edge_rates) * unif_rate_factor;
    auto P = probMatrix(rg, edge_rates, unif_rate);

    auto n = rg.nodeCount();

    auto prob = Vector(n);
    for (auto pair : init_probs)
    {
        prob(pair.idx) = pair.prob;
    }
    Vector prob_old = prob;
    Vector check_point = prob;

    auto [ltp, rtp, poi_probs] = poissonTruncPoint(unif_rate * time, tol);

    for (uint k = 0; k < ltp; k++)
    {
        std::swap(prob, prob_old);
        dot(P, prob_old, mutableView(prob));
        if ((k + 1) % ss_check_interval == 0)
        {
            if (maxDiff(check_point, prob) < tol)
            {
                return prob;
            }
            check_point = prob;
        }
    }
    Real term_sum = 0.0;
    auto prob_t = Vector(n, 0.0);

    for (uint k = ltp; k <= rtp; k++)
    {
        Real term = poi_probs[k - ltp];
        term_sum += term;
        blas::axpy(term, prob, mutableView(prob_t));

        std::swap(prob, prob_old);
        dot(P, prob_old, mutableView(prob));
        if ((k - ltp + 1) % ss_check_interval == 0)
        {
            if (maxDiff(check_point, prob) < tol)
            {
                blas::axpy(1.0 - term_sum, prob, mutableView(prob_t));
                return prob_t;
            }
            check_point = prob;
        }
    }

    return prob_t;
}

}  // namespace sanity::petrinet
