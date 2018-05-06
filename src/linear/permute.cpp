#include "permute.hpp"
#include <cassert>
namespace sanity::linear
{
Permutation::Permutation(std::vector<uint> perm, bool is_forward)
    : _forward(perm.size())
{
    for (uint i = 0; i < perm.size(); i++)
    {
        assert(perm[i] >= 0);
        _forward[i] = (int)perm[i];
    }
    int max_num = 0;
    for (uint i = 0; i < _forward.size(); i++)
    {
        if (_forward[i] > max_num)
        {
            max_num = _forward[i];
        }
    }
    _backward = std::vector<int>((uint)(max_num + 1), -1);
    for (uint i = 0; i < _forward.size(); i++)
    {
        _backward[(uint)_forward[i]] = (int)i;
    }
    if (!is_forward)
    {
        std::swap(_forward, _backward);
    }
}
}  // namespace sanity::linear
