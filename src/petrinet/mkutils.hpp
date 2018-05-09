#pragma once
#include <cassert>
#include <map>
#include "graph.hpp"
#include "petrinet.hpp"
#include "type.hpp"

namespace sanity::petrinet
{
struct CompMarking
{
    uint nplace;
    CompMarking(uint nplace) : nplace(nplace) {}
    bool operator()(const Token* m1, const Token* m2)
    {
        assert(m1 != nullptr);
        assert(m2 != nullptr);
        for (uint i = 0; i < nplace; i++)
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

void printMarking(const Marking& mk);
int findMarking(std::map<const Token*, uint, CompMarking>& mk_map,
                const Marking& newmk);
uint addNewMarking(graph::DiGraph& graph,
                   std::map<const Token*, uint, CompMarking>& mk_map,
                   std::vector<Marking>& markings, Marking&& newmk);
}  // namespace sanity::petrinet
