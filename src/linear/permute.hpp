#pragma once
#include <vector>
#include "type.hpp"
namespace sanity::linear
{
class Permutation
{
    std::vector<int> _forward;
    std::vector<int> _backward;

public:
    Permutation(std::vector<uint> perm, bool is_forward);
    Permutation(uint size) : _forward(size), _backward(size)
    {
        for (uint i = 0; i < size; i++)
        {
            _forward[i] = (int)i;
        }
        for (uint i = 0; i < size; i++)
        {
            _backward[i] = (int)i;
        }
    }
    int forward(uint org_idx) const
    {
        if (org_idx > maxOrgIdx())
        {
            return -1;
        }
        return _forward[org_idx];
    }
    int backward(uint permuted_idx) const
    {
        if (permuted_idx > maxPermutedIdx())
        {
            return -1;
        }
        return _backward[permuted_idx];
    }
    uint maxPermutedIdx() const { return _backward.size() - 1; }
    uint maxOrgIdx() const { return _forward.size() - 1; }
};
}  // namespace sanity::linear
