#include "mkutils.hpp"
#include <iostream>
namespace sanity::petrinet
{
using namespace graph;

int findMarking(MarkingMap& mk_map, const MarkingIntf* newmk)
{
    auto res = mk_map.find(newmk);
    if (res == mk_map.end())
    {  // new token
        return -1;
    }
    else
    {
        return (int)res->second;
    }
}

uint addNewMarking(graph::DiGraph& graph, MarkingMap& mk_map,
                   std::vector<std::unique_ptr<MarkingIntf>>& markings,
                   std::unique_ptr<MarkingIntf> newmk)
{
    uint idx = graph.addNode();
    mk_map[newmk.get()] = idx;
    markings.push_back(std::move(newmk));
    return idx;
}

}  // namespace sanity::petrinet
