#include "srnssolve.hpp"
#include "graph.hpp"
#include "linear.hpp"
#include "splinear.hpp"
#include "srnreach.hpp"

namespace sanity::petrinet
{
using namespace linear;
using namespace splinear;
using namespace graph;

Spmatrix srnRateMatrix(const DiGraph& reach_graph,
                       const std::vector<Real>& edge_rates)
{
    const uint n = reach_graph.nodeCount();
    auto spmat = SpmatrixCreator(n, n);
    for (uint j = 0; j < n; j++)
    {
        const auto& node = reach_graph.getNode(j);
        for (const auto& edge : node.edges)
        {
            Real rate = edge_rates[edge.eid];
            uint i = edge.dst;
            spmat.addEntry((uint)i, j, rate);
            spmat.addEntry(j, j, -rate);
        }
    }
    return spmat.create(Spmatrix::RowCompressed);
}

IterationResult srnSteadyStateSor(const Spmatrix& Q, VectorMutableView prob,
                                  Real w, Real tol, uint max_iter)
{
    assert(Q.format == Spmatrix::RowCompressed);
    uint iter;
    Real error;
    scale(1.0 / norm1(prob), prob);
    auto prob_prev = Vector(prob.size());
    for (iter = 1; iter <= max_iter; iter++)
    {
        copy(constView(prob), mutableView(prob_prev));
        for (uint i = 0; i < Q.nrow; i++)
        {
            auto iter = initRowIter(Q, i);
            Real residual = 0;
            Real a_ii = 0;
            while (!iter.end())
            {
                if (iter.col() == i)  // diag
                {
                    a_ii = iter.val();
                }
                else
                {  // non diag
                    residual -= iter.val() * prob(iter.col());
                }
                iter.nextNonzero();
            }
            prob(i) = w * residual / a_ii + (1 - w) * prob(i);
        }
        scale(1.0 / norm1(prob), prob);
        error = maxDiff(prob, prob_prev);
        if (error < tol)
        {
            break;
        }
    }
    return {.error = error, .nIter = iter};
}

IrreducibleSrnSteadyStateSol srnSteadyStateSor(
    const graph::DiGraph& rg, const std::vector<Real>& edge_rates, Real w,
    Real tol, uint max_iter)
{
    auto Q = srnRateMatrix(rg, edge_rates);
    Vector prob(rg.nodeCount(), 1.0);
    if (rg.nodeCount() == 1)
    {
        return IrreducibleSrnSteadyStateSol{std::move(prob)};
    }
    auto res = srnSteadyStateSor(Q, mutableView(prob), w, tol, max_iter);
    if (res.error > tol || std::isnan(res.error))
    {
        std::ostringstream os;
        os << "SOR failed to converge in srnSteadyStateSor with error = "
           << res.error << ", nIter = " << res.nIter << ".";
        throw std::invalid_argument(os.str());
    }
    return IrreducibleSrnSteadyStateSol{std::move(prob)};
}

IterationResult srnSteadyStatePower(const splinear::Spmatrix& P,
                                    linear::VectorMutableView prob, Real tol,
                                    uint max_iter)
{
    Real n1 = norm1(prob);
    scale(1.0 / n1, mutableView(prob));

    auto next_prob = Vector(prob.size());
    uint iter;
    Real error;
    for (iter = 1; iter <= max_iter; iter++)
    {
        dot(P, prob, mutableView(next_prob));
        Real n1 = norm1(next_prob);
        scale(1.0 / n1, mutableView(next_prob));
        error = maxDiff(prob, next_prob);
        copy(next_prob, prob);
        if (error < tol)
        {
            break;
        }
    }
    return {.error = error, .nIter = iter};
}

struct StateReordering
{
    uint ntan;  // number of tangible states
    Permutation mat2node;
    std::vector<uint> nstatesList;  // number of bottom scc states
};

static StateReordering reorderState(const DiGraph& rg)
{
    auto scc_list = decompScc(rg);
    std::vector<uint> mat2node;
    mat2node.reserve(rg.nodeCount());
    std::vector<uint> nstates;
    uint ntan = 0;
    for (const auto& scc : scc_list)
    {
        if (!scc.isBottom)
        {
            mat2node.insert(mat2node.end(), scc.nodes.begin(),
                            scc.nodes.end());
            ntan += scc.nodes.size();
        }
    }
    for (const auto& scc : scc_list)
    {
        if (scc.isBottom)
        {
            mat2node.insert(mat2node.end(), scc.nodes.begin(),
                            scc.nodes.end());
            nstates.push_back(scc.nodes.size());
        }
    }
    return {ntan, Permutation(std::move(mat2node), true), std::move(nstates)};
}

static Spmatrix ttRateMatrix(const DiGraph& rg,
                             const std::vector<Real>& edge_rates,
                             const Permutation& mat2node, uint ntan)
{
    auto spmat = SpmatrixCreator(ntan, ntan);
    for (uint j = 0; j < ntan; j++)
    {
        int nid = mat2node.forward(j);
        assert(nid >= 0);
        const auto& node = rg.getNode((uint)nid);
        for (const auto& edge : node.edges)
        {
            Real rate = edge_rates[edge.eid];
            int i = mat2node.backward(edge.dst);
            assert(i >= 0);
            if ((uint)i < ntan)
            {
                spmat.addEntry((uint)i, j, rate);
                spmat.addEntry(j, j, -rate);
            }
            else
            {  // dst is not in the matrix
                spmat.addEntry(j, j, -rate);
            }
        }
    }
    return spmat.create(Spmatrix::RowCompressed);
}

static Spmatrix taRateMatrix(const DiGraph& rg,
                             const std::vector<Real>& edge_rates,
                             const Permutation& mat2node, uint ntan,
                             uint abs_start, uint abs_end)
{
    assert(abs_start >= ntan);
    assert(abs_end > abs_start);
    uint nabs = abs_end - abs_start;
    auto spmat = SpmatrixCreator(nabs, ntan);
    for (uint j = 0; j < ntan; j++)
    {
        int nid = mat2node.forward(j);
        assert(nid >= 0);
        const auto& node = rg.getNode((uint)nid);
        for (const auto& edge : node.edges)
        {
            Real rate = edge_rates[edge.eid];
            int i = mat2node.backward(edge.dst);
            assert(i >= 0);
            if ((uint)i >= abs_start && (uint)i < abs_end)
            {
                spmat.addEntry((uint)i - abs_start, j, rate);
            }
        }
    }
    return spmat.create(Spmatrix::RowCompressed);
}

// the last row is replaced with all 1s
static Spmatrix aaRateMatrix(const DiGraph& rg,
                             const std::vector<Real>& edge_rates,
                             const Permutation& mat2node, uint abs_start,
                             uint abs_end)
{
    assert(abs_end > abs_start);
    uint nabs = abs_end - abs_start;
    auto spmat = SpmatrixCreator(nabs, nabs);
    for (uint j = abs_start; j < abs_end; j++)
    {
        int nid = mat2node.forward(j);
        assert(nid >= 0);
        const auto& node = rg.getNode((uint)nid);
        Real total_rate = 0.0;
        for (const auto& edge : node.edges)
        {
            Real rate = edge_rates[edge.eid];
            int i = mat2node.backward(edge.dst);
            assert(i >= 0);
            assert((uint)i >= abs_start);
            total_rate += rate;
            if ((uint)i != abs_end - 1)  // excluding the last row
            {
                spmat.addEntry((uint)i - abs_start, j - abs_start, rate);
            }
        }
        if (j != abs_end - 1)
        {
            spmat.addEntry(j - abs_start, j - abs_start, -total_rate);
        }
        spmat.addEntry(abs_end - 1 - abs_start, j - abs_start,
                       1.0);  // adding the last
                              // row
    }
    return spmat.create(Spmatrix::RowCompressed);
}

GeneralSrnSteadyStateSol srnSteadyStateDecomp(
    const graph::DiGraph& rg, const std::vector<Real>& edge_rates,
    const std::vector<MarkingInitProb>& init_probs,
    const std::function<void(const splinear::Spmatrix& A,
                             linear::VectorMutableView x,
                             linear::VectorConstView b)>& spsolver)

{
    auto reorder = reorderState(rg);
    Vector solution(rg.nodeCount(), 0.0);
    for (const auto& mkp : init_probs)
    {
        int mat_idx = reorder.mat2node.backward(mkp.idx);
        assert(mat_idx >= 0);
        if ((uint)mat_idx < reorder.ntan)
        {
            solution((uint)mat_idx) = -mkp.prob;
        }
        else
        {
            solution((uint)mat_idx) = mkp.prob;
        }
    }

    // solving Q_TT * tau = - pi(0)
    if (reorder.ntan > 0)
    {
        auto QTT =
            ttRateMatrix(rg, edge_rates, reorder.mat2node, reorder.ntan);
        auto sol = blockView(mutableView(solution), 0, reorder.ntan);
        auto b = Vector(sol);
        // guess a solution
        fill(1.0, sol);
        spsolver(QTT, sol, b);
    }

    uint abs_start = reorder.ntan;
    for (uint nabs : reorder.nstatesList)
    {
        // sol(0, ntan) = Q_TA * tau
        uint abs_end = abs_start + nabs;
        if (reorder.ntan > 0)
        {
            auto QTA = taRateMatrix(rg, edge_rates, reorder.mat2node,
                                    reorder.ntan, abs_start, abs_end);
            auto sol = blockView(mutableView(solution), abs_start, nabs);
            dotpx(QTA, blockView(solution, 0, reorder.ntan), sol);
        }

        // Q_AA * sol(abs_start, abs_end) = [0,0, ..., total_prob]^T
        Real total_prob = 0.0;
        for (uint i = abs_start; i < abs_end; i++)
        {
            total_prob += solution(i);
        }
        {
            auto QAA = aaRateMatrix(rg, edge_rates, reorder.mat2node,
                                    abs_start, abs_end);

            // right hand side
            auto b = Vector(nabs, 0.0);
            b(nabs - 1) = 1.0;

            // guess (1/n, 1/n, ..., 1/n)
            auto sol = blockView(mutableView(solution), abs_start, nabs);
            fill(1.0 / (Real)nabs, sol);
            spsolver(QAA, sol, b);
            scale(total_prob, sol);
        }
        abs_start = abs_end;
    }

    return {.matrix2node = std::move(reorder.mat2node),
            .nTransient = reorder.ntan,
            .solution = std::move(solution)};
}
}  // namespace sanity::petrinet
