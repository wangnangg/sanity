#include "reach.hpp"
#include <cassert>
#include <map>
#include "mkutils.hpp"

namespace sanity::petrinet
{
using namespace graph;
using namespace linear;

ReachGraph genReachGraph(const PetriNet& net, const MarkingIntf& mk)
{
    DiGraph graph;
    std::vector<std::unique_ptr<MarkingIntf>> markings;
    std::vector<uint> edge2trans;
    MarkingMap mk_map;

    auto init_mk = mk.clone();
    addNewMarking(graph, mk_map, markings, std::move(init_mk));

    size_t curr_nid = 0;
    while (curr_nid < markings.size())
    {
        for (uint tid : net.enabledTransitions(markings[curr_nid].get()))
        {
            auto newmk = net.fireTransition(tid, markings[curr_nid].get());
            int dst = findMarking(mk_map, newmk.get());
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
