#include "reach.hpp"
#include <cassert>
#include <map>

namespace sanity::petrinet
{
using namespace graph;
using namespace linear;

void printMarking(const Token* tokens, uint size)
{
    std::cout << "( ";
    for (uint i = 0; i < size; i++)
    {
        std::cout << tokens[i] << " ";
    }
    std::cout << ")" << std::endl;
}

struct CompMarking
{
    uint nplace;
    CompMarking(uint nplace) : nplace(nplace) {}
    bool operator()(const Token* m1, const Token* m2)
    {
        assert(m1 != nullptr);
        assert(m2 != nullptr);
        for (size_t i = 0; i < nplace; i++)
        {
            if (m1[i] < m2[i])
            {
                return true;
            }
            else if (m1[i] > m2[i])
            {
                return false;
            }
        }
        return false;
    }
};

int findMarking(std::map<const Token*, uint, CompMarking>& mk_map,
                const Marking& newmk)
{
    auto res = mk_map.find(&newmk.nToken(0));
    if (res == mk_map.end())
    {  // new token
        return -1;
    }
    else
    {
        return (int)res->second;
    }
}

uint addNewMarking(DiGraph& graph,
                   std::map<const Token*, uint, CompMarking>& mk_map,
                   std::vector<Marking>& markings, Marking&& newmk)
{
    uint idx = graph.addNode();
    mk_map[&newmk.nToken(0)] = idx;
    markings.push_back(std::move(newmk));
    return idx;
}

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
    return {.digraph = std::move(graph),
            .markings = std::move(markings),
            .edge2trans = std::move(edge2trans)};
}

}  // namespace sanity::petrinet
