#include "srnreach.hpp"
#include <cassert>
#include <map>
#include "graph.hpp"
#include "linear.hpp"

namespace sanity::petrinet
{
using namespace linear;
using namespace graph;

linear::SpmatrixTriple srnRateMatrix(const StochasticRewardNet& srn,
                                     const ReachGraph& g,
                                     const linear::Permutation& mat2rg_idx)
{
    const uint n = mat2rg_idx.maxOrgIdx() + 1;
    auto spmat = SpmatrixTriple(n, n);
    for (uint j = 0; j < n; j++)
    {
        int nid = mat2rg_idx.forward(j);
        assert(nid >= 0);
        const auto& src_mk = g.markings[(uint)nid];
        const auto& node = g.digraph.getNode((uint)nid);
        for (const auto& eid : node.edgeIdxList)
        {
            const auto& edge = g.digraph.getEdge(eid);
            uint tid = g.edge2trans[eid];
            const auto& prop = srn.transProps[tid];
            assert(prop.type == SRNTransType::Exponetial);
            auto state = State{&srn.pnet, &src_mk};
            Real rate = prop.val(state);
            int i = mat2rg_idx.backward(edge.dst);
            if (i >= 0)
            {
                spmat.triples.push_back({(uint)i, j, rate});
                spmat.triples.push_back({j, j, -rate});
            }
            else
            {  // dst is not in the matrix
                spmat.triples.push_back({j, j, -rate});
            }
        }
    }
    sortTriples(spmat);
    return spmat;
}

bool isVanMarking(const StochasticRewardNet& srn, const Marking& mk)
{
    int tid = srn.pnet.firstEanbledTrans(mk);
    if (tid < 0)
    {
        return false;
    }
    else
    {
        if (srn.transProps[(uint)tid].type == SRNTransType::Immediate)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

struct TangibleReachingRate
{
    Marking tanMarking;
    Real rate;
};

std::vector<TangibleReachingRate> eliminateVanMarking(
    const StochasticRewardNet& srn, const Marking& vanmk, Real inrate);

ReducedReachGenResult srnGenReducedReach(const StochasticRewardNet& srn,
                                         const Marking& mk);

}  // namespace sanity::petrinet
