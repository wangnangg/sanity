#include "utils.hpp"
namespace sanity::linear
{
Real maxDiff(VectorView<Real, Const> v1, VectorView<Real, Const> v2)
{
    assert(v1.size() == v2.size());
    int n = v1.size();
    Real max = 0;
    for (int i = 0; i < n; i++)
    {
        Real v = std::abs(get(v1, i) - get(v2, i));
        if (max < v)
        {
            max = v;
        }
    }
    return max;
}
Real maxDiff(VectorView<Complex, Const> v1, VectorView<Complex, Const> v2)
{
    assert(v1.size() == v2.size());
    int n = v1.size();
    Real max = 0;
    for (int i = 0; i < n; i++)
    {
        Real v = std::abs(get(v1, i) - get(v2, i));
        if (max < v)
        {
            max = v;
        }
    }
    return max;
}
}
