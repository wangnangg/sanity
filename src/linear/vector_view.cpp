#include "vector_view.hpp"
namespace sanity::linear
{
VectorConstView blockView(VectorConstView v, int i, int size)
{
    return VectorConstView(&v(i), v.inc(), v.size());
}

VectorMutableView blockView(VectorMutableView v, int i, int size)
{
    return VectorMutableView(&v(i), v.inc(), v.size());
}
}
