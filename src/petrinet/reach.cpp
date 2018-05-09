#include "reach.hpp"
#include <cassert>
#include <map>
#include "mkutils.hpp"

namespace sanity::petrinet
{
using namespace graph;
using namespace linear;

ReachGraph genReachGraph(const PetriNet& net, const Marking& mk)
{
    DiGraph graph;
    std::vector<Marking> markings;
    std::vector<uint> edge2trans;
    uint nplace = mk.size();
    std::map<const Token*, uint, CompMarking> mk_map(CompMarking{
        nplace});  // map from a marking to its position in markings

    auto init_mk = mk.clone();
    addNewMarking(graph, mk_map, markings, std::move(init_mk));

    size_t curr_nid = 0;
    while (curr_nid < markings.size())
    {
        for (uint tid : net.enabledTransitions(markings[curr_nid]))
        {
            Marking newmk = net.fireTransition(tid, markings[curr_nid]);
            int dst = findMarking(mk_map, newmk);
            if (dst < 0)
            {
                dst = (int)addNewMarking(graph, mk_map, markings,
                                         std::move(newmk));
            }
            edge2trans.push_back(tid);
            graph.addEdge(curr_nid, (uint)dst);
        }
        curr_nid += 1;
    }
    return {.graph = std::move(graph),
            .nodeMarkings = std::move(markings),
            .edgeTrans = std::move(edge2trans)};
}

}  // namespace sanity::petrinet
