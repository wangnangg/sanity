#include "mkutils.hpp"
#include <iostream>
namespace sanity::petrinet
{
using namespace graph;

int findMarking(MarkingMap& mk_map, const Marking& newmk)
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

uint addNewMarking(DiGraph& graph, MarkingMap& mk_map,
                   std::vector<Marking>& markings, Marking&& newmk)
{
    uint idx = graph.addNode();
    mk_map[&newmk.nToken(0)] = idx;
    markings.push_back(std::move(newmk));
    return idx;
}

}  // namespace sanity::petrinet
