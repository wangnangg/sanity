#pragma once

#include "graph.hpp"
#include "linear.hpp"
#include "splinear.hpp"
#include "srn.hpp"
#include "srnreach.hpp"
#include "type.hpp"

namespace sanity::petrinet
{
splinear::Spmatrix srnRateMatrix(const graph::DiGraph& reach_graph,
                                 const std::vector<Real>& edge_rates,
                                 const linear::Permutation& mat2rg_idx);

// SOR method, only good for irreducible Markov chains
IterationResult srnSteadyStateSor(const splinear::Spmatrix& Q,
                                  linear::VectorMutableView prob, Real w,
                                  Real tol, uint max_iter);

// Power method, P needs to be a unified prob matrix
IterationResult srnSteadyStatePower(const splinear::Spmatrix& P,
                                    linear::VectorMutableView prob, Real tol,
                                    uint max_iter);

struct SrnSteadyStateSolution
{
    linear::Permutation
        matrix2node;  // the first n in the matrix are tangibles and the
                      // rest are absorbing components.
    uint nTransient;
    linear::Vector solution;  // the solution is in matrix order. for
                              // tangibles, the values are cumulative times.
                              // for absorbings, the values are probabilities.
};

// decomposition method
SrnSteadyStateSolution srnSteadyStateDecomp(
    const graph::DiGraph& rg, const std::vector<Real>& edge_rates,
    const std::vector<MarkingInitProb>& init_probs,
    const std::function<void(const splinear::Spmatrix& A,
                             linear::VectorMutableView x,
                             linear::VectorConstView b)>& spsolver);

}  // namespace sanity::petrinet
