#include "srnreach.hpp"
#include <cassert>
#include <iostream>
#include <map>
#include <stdexcept>
#include "graph.hpp"
#include "linear.hpp"
#include "mkutils.hpp"
#include "srnssolve.hpp"

namespace sanity::petrinet
{
using namespace linear;
using namespace splinear;
using namespace graph;

bool isVanMarking(const StochasticRewardNet& srn, const MarkingIntf* mk)
{
    int tid = srn.firstEanbledTrans(mk);
    if (tid < 0)
    {
        return false;
    }
    else
    {
        if (srn.transProps[(uint)tid].type == SrnTransType::Immediate)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

struct MarkingListProb
{
    std::vector<std::unique_ptr<MarkingIntf>> markings;
    std::vector<Real> prob;
};

static Spmatrix srnProbMatrix(const DiGraph& reach_graph,
                              const std::vector<Real>& edge_probs)
{
    const uint n = reach_graph.nodeCount();
    auto spmat = SpmatrixCreator(n, n);
    for (uint j = 0; j < n; j++)
    {
        const auto& node = reach_graph.getNode((uint)j);
        if (node.edges.size() > 0)
        {
            for (const auto& edge : node.edges)
            {
                Real prob = edge_probs[edge.eid];
                uint i = edge.dst;
                spmat.addEntry(i, j, prob);
            }
        }
        else
        {
            spmat.addEntry(j, j, 1.0);
        }
    }
    return spmat.create(Spmatrix::RowCompressed);
}

MarkingListProb eliminateVanMarking(const StochasticRewardNet& srn,
                                    std::unique_ptr<MarkingIntf> vanmk,
                                    Real tol, uint max_iter)
{
    DiGraph graph;
    std::vector<std::unique_ptr<MarkingIntf>> node_markings;
    std::vector<Real> edge_probs;
    std::vector<uint> tan_mk_ids;
    MarkingMap mk_map;

    node_markings.push_back(std::move(vanmk));
    graph.addNode();

    size_t curr_nid = 0;
    while (curr_nid < node_markings.size())
    {
        if (isVanMarking(srn, node_markings[curr_nid].get()))
        {
            Real prob_sum = 0.0;
            for (uint tid :
                 srn.enabledTransitions(node_markings[curr_nid].get()))
            {
                assert(srn.transProps[tid].type == SrnTransType::Immediate);
                Real p_weight = srn.transProps[tid].val(
                    {&srn, node_markings[curr_nid].get()});
                auto newmk =
                    srn.fireTransition(tid, node_markings[curr_nid].get());
                int dst = findMarking(mk_map, newmk.get());
                if (dst < 0)
                {
                    dst = (int)addNewMarking(graph, mk_map, node_markings,
                                             std::move(newmk));
                }
                edge_probs.push_back(p_weight);
                prob_sum += p_weight;
                graph.addEdge(curr_nid, (uint)dst);
            }
            // normalize probs
            for (const auto& edge : graph.getNode(curr_nid).edges)
            {
                edge_probs[edge.eid] *= 1.0 / prob_sum;
            }
        }
        else
        {
            tan_mk_ids.push_back(curr_nid);
        }
        curr_nid++;
    }

    auto P = srnProbMatrix(graph, edge_probs);
    auto reaching_probs = Vector(graph.nodeCount());
    reaching_probs(0) = 1;
    auto iter_res =
        srnSteadyStatePower(P, mutableView(reaching_probs), tol, max_iter);
    if (iter_res.error > tol)
    {
        throw std::invalid_argument(
            "The solution for the embedded DTMC does not converge. Is it "
            "periodic?");
    }
    Real prob_sum = 0.0;
    MarkingListProb problist;
    for (uint tan_id : tan_mk_ids)
    {
        problist.markings.push_back(std::move(node_markings[tan_id]));
        problist.prob.push_back(reaching_probs(tan_id));
        prob_sum += reaching_probs(tan_id);
    }
    // prob_sum should be near 1
    assert(std::abs(prob_sum - 1.0) <= tol);
    return problist;
}

ReducedReachGenResult genReducedReachGraph(const StochasticRewardNet& srn,
                                           const MarkingIntf& mk)
{
    uint max_iter = 1000;
    Real tol = 1e-6;

    DiGraph graph;
    std::vector<std::unique_ptr<MarkingIntf>> node_markings;
    std::vector<Real> edge_rates;
    std::vector<MarkingInitProb> init_probs;
    MarkingMap mk_map;  // map from a marking to its position in markings

    auto init_mk = mk.clone();
    if (isVanMarking(srn, init_mk.get()))
    {
        auto mark_prob_list =
            eliminateVanMarking(srn, std::move(init_mk), tol, max_iter);
        uint n = mark_prob_list.markings.size();
        for (uint i = 0; i < n; i++)
        {
            int dst = findMarking(mk_map, mark_prob_list.markings[i].get());
            if (dst < 0)
            {
                dst =
                    (int)addNewMarking(graph, mk_map, node_markings,
                                       std::move(mark_prob_list.markings[i]));
            }
            init_probs.push_back({(uint)dst, mark_prob_list.prob[i]});
        }
    }
    else
    {
        init_probs.push_back({0, 1.0});
        addNewMarking(graph, mk_map, node_markings, std::move(init_mk));
    }

    uint curr_nid = 0;
    while (curr_nid < node_markings.size())
    {
        for (uint tid : srn.enabledTransitions(node_markings[curr_nid].get()))
        {
            assert(srn.transProps[tid].type == SrnTransType::Exponetial);
            Real t_rate = srn.transProps[tid].val(
                {&srn, node_markings[curr_nid].get()});
            auto newmk =
                srn.fireTransition(tid, node_markings[curr_nid].get());
            if (isVanMarking(srn, newmk.get()))
            {
                auto mark_prob_list =
                    eliminateVanMarking(srn, std::move(newmk), tol, max_iter);
                uint n = mark_prob_list.markings.size();
                for (uint i = 0; i < n; i++)
                {
                    int dst =
                        findMarking(mk_map, mark_prob_list.markings[i].get());
                    if (dst < 0)
                    {
                        dst = (int)addNewMarking(
                            graph, mk_map, node_markings,
                            std::move(mark_prob_list.markings[i]));
                    }
                    edge_rates.push_back(t_rate * mark_prob_list.prob[i]);
                    graph.addEdge(curr_nid, (uint)dst);
                }
            }
            else
            {
                int dst = findMarking(mk_map, newmk.get());
                if (dst < 0)
                {
                    dst = (int)addNewMarking(graph, mk_map, node_markings,
                                             std::move(newmk));
                }
                edge_rates.push_back(t_rate);
                graph.addEdge(curr_nid, (uint)dst);
            }
        }
        curr_nid += 1;
    }

    return {.graph = std::move(graph),
            .nodeMarkings = std::move(node_markings),
            .edgeRates = std::move(edge_rates),
            .initProbs = std::move(init_probs)};
}

}  // namespace sanity::petrinet
