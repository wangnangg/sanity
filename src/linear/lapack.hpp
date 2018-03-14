#include "matrix.hpp"
#include "matrix_view.hpp"
#include "vector.hpp"

namespace sanity::linear::lapack
{
int geqrf(MatrixMutView<Real, General> A, VectorMutView<Real> tau);
int geqrf(MatrixMutView<Complex, General> A, VectorMutView<Complex> tau);

int orgqr(MatrixMutView<Real, General> mA, VectorView<Real> tau);
int orgqr(MatrixMutView<Complex, General> mA, VectorView<Complex> tau);
}
