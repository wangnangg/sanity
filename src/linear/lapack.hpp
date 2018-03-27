#include "matrix.hpp"
#include "matrix_view.hpp"
#include "vector.hpp"

namespace sanity::linear::lapack
{
int geqrf(MatrixMutableView A, VectorMutableView tau);
int orgqr(MatrixMutableView mA, VectorConstView tau);
}
