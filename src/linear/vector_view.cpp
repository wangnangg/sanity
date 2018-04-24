#include "vector_view.hpp"
namespace sanity::linear
{
VectorConstView blockView(VectorConstView v, int i, int size)
{
    if (size < 0)
    {
        size += v.size() - i + 1;
    }
    return VectorConstView(&v(i), v.inc(), size);
}

VectorMutableView blockView(VectorMutableView v, int i, int size)
{
    if (size < 0)
    {
        size += v.size() - i + 1;
    }
    return VectorMutableView(&v(i), v.inc(), size);
}

CVectorConstView blockView(CVectorConstView v, int i, int size)
{
    if (size < 0)
    {
        size += v.size() - i + 1;
    }
    return CVectorConstView(&v(i), v.inc(), size);
}
CVectorMutableView blockView(CVectorMutableView v, int i, int size)
{
    if (size < 0)
    {
        size += v.size() - i + 1;
    }
    return CVectorMutableView(&v(i), v.inc(), size);
}
}  // namespace sanity::linear
