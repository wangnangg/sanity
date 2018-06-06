#pragma once
#include <cassert>
#include <functional>
#include <unordered_map>
#include "graph.hpp"
#include "petrinet.hpp"
#include "type.hpp"

namespace sanity::petrinet
{
struct HashMarking
{
    std::size_t operator()(const MarkingIntf* m1) const { return m1->hash(); }
};

struct MarkingEqual
{
    bool operator()(const MarkingIntf* m1, const MarkingIntf* m2) const
    {
        return m1->equal(m2);
    }
};

using MarkingMap =
    std::unordered_map<const MarkingIntf*, uint, HashMarking, MarkingEqual>;

int findMarking(MarkingMap& mk_map, const MarkingIntf* newmk);

uint addNewMarking(graph::DiGraph& graph, MarkingMap& mk_map,
                   std::vector<std::unique_ptr<MarkingIntf>>& markings,
                   std::unique_ptr<MarkingIntf> newmk);

}  // namespace sanity::petrinet
