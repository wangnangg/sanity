#include "matrix.hpp"
#include "matrix_view.hpp"
#include "vector.hpp"

namespace sanity::linear::lapack
{
int geqrf(MatrixView<Real, General, Mutable> A, VectorView<Real, Mutable> tau);
int orgqr(MatrixView<Real, General, Mutable> mA, VectorView<Real, Const> tau);
}
