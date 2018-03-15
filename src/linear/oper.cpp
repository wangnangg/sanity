#include "oper.hpp"
#include <algorithm>

namespace sanity::linear
{
Real maxDiff(VectorView<Real> v1, VectorView<Real> v2)
{
    assert(v1.size() == v2.size());
    int n = v1.size();
    Real max = 0;
    for (int i = 0; i < n; i++)
    {
        Real v = std::abs(v1(i) - v2(i));
        if (max < v)
        {
            max = v;
        }
    }
    return max;
}
Real maxDiff(VectorView<Complex> v1, VectorView<Complex> v2)
{
    assert(v1.size() == v2.size());
    int n = v1.size();
    Real max = 0;
    for (int i = 0; i < n; i++)
    {
        Real v = std::abs(v1(i) - v2(i));
        if (max < v)
        {
            max = v;
        }
    }
    return max;
}
}
