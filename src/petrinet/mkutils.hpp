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
    uint nplace;
    std::hash<std::string_view> hasher;
    HashMarking(uint nplace) : nplace(nplace) {}

    std::size_t operator()(const Token* m1) const
    {
        std::string_view sview((const char*)m1, sizeof(Token) * nplace);
        return hasher(sview);
    }
};

struct MarkingEqual
{
    uint nplace;
    MarkingEqual(uint nplace) : nplace(nplace) {}

    bool operator()(const Token* m1, const Token* m2) const
    {
        for (uint i = 0; i < nplace; i++)
        {
            if (m1[i] != m2[i])
            {
                return false;
            }
        }
        return true;
    }
};

using MarkingMap =
    std::unordered_map<const Token*, uint, HashMarking, MarkingEqual>;

int findMarking(MarkingMap& mk_map, const Marking& newmk);

uint addNewMarking(graph::DiGraph& graph, MarkingMap& mk_map,
                   std::vector<Marking>& markings, Marking&& newmk);

}  // namespace sanity::petrinet
