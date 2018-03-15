#include "matrix_view.hpp"
#include "vector_view.hpp"
namespace sanity::linear
{
Real maxDiff(VectorView<Real> v1, VectorView<Real> v2);
Real maxDiff(VectorView<Complex> v1, VectorView<Complex> v2);
}
