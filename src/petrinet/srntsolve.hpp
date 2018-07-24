#include "graph.hpp"
#include "linear.hpp"
#include "splinear.hpp"
#include "srn.hpp"
#include "srnreach.hpp"
#include "type.hpp"

namespace sanity::petrinet
{
splinear::Spmatrix srnProbMatrix(const graph::DiGraph& reach_graph,
                                 const std::vector<Real>& edge_rates,
                                 Real unif_rate);

linear::Vector srnTransientProb(
    const graph::DiGraph& rg, const std::vector<Real>& edge_rates,
    const std::vector<MarkingInitProb>& init_probs, Real time,
    Real unif_rate_factor = 1.05, Real tol = 1e-6,
    uint ss_check_interval = 10);

}  // namespace sanity::petrinet
