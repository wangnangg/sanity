#include "vector_view.hpp"
#include <cassert>
namespace sanity::linear
{
VectorConstView blockView(VectorConstView v, uint i, uint size)
{
    assert(i + size <= v.size());
    return VectorConstView(&v(i), v.inc(), size);
}

VectorMutableView blockView(VectorMutableView v, uint i, uint size)
{
    assert(i + size <= v.size());
    return VectorMutableView(&v(i), v.inc(), size);
}

CVectorConstView blockView(CVectorConstView v, uint i, uint size)
{
    assert(i + size <= v.size());
    return CVectorConstView(&v(i), v.inc(), size);
}
CVectorMutableView blockView(CVectorMutableView v, uint i, uint size)
{
    assert(i + size <= v.size());
    return CVectorMutableView(&v(i), v.inc(), size);
}
}  // namespace sanity::linear
