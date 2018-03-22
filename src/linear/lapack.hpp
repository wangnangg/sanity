#include "matrix.hpp"
#include "matrix_view.hpp"
#include "vector.hpp"

namespace sanity::linear::lapack
{
int geqrf(MatrixView<Real, General, RowMajor, NoConj, Mutable> A,
          VectorView<Real, NoConj, Mutable> tau);
int geqrf(MatrixView<Real, General, ColMajor, NoConj, Mutable> A,
          VectorView<Real, NoConj, Mutable> tau);

int orgqr(MatrixView<Real, General, RowMajor, NoConj, Mutable> mA,
          VectorView<Real, NoConj, Const> tau);
int orgqr(MatrixView<Real, General, ColMajor, NoConj, Mutable> mA,
          VectorView<Real, NoConj, Const> tau);
}
