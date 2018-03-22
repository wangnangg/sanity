#include "lapack.hpp"
#define HAVE_LAPACK_CONFIG_H
#define LAPACK_COMPLEX_CPP
#include "lapack/lapacke.h"

namespace sanity::linear::lapack
{
int geqrf(MatrixView<Real, General, RowMajor, NoConj, Mutable> A,
          VectorView<Real, NoConj, Mutable> tau)
{
    return LAPACKE_dgeqrf(LAPACK_ROW_MAJOR, A.nRow(), A.nCol(), A.data(),
                          A.lDim(), tau.data());
}
int geqrf(MatrixView<Real, General, ColMajor, NoConj, Mutable> A,
          VectorView<Real, NoConj, Mutable> tau)
{
    return LAPACKE_dgeqrf(LAPACK_COL_MAJOR, A.nRow(), A.nCol(), A.data(),
                          A.lDim(), tau.data());
}

int orgqr(MatrixView<Real, General, RowMajor, NoConj, Mutable> mA,
          VectorView<Real, NoConj, Const> tau)
{
    return LAPACKE_dorgqr(LAPACK_ROW_MAJOR, mA.nRow(), tau.size(), tau.size(),
                          mA.data(), mA.lDim(), tau.data());
}
int orgqr(MatrixView<Real, General, ColMajor, NoConj, Mutable> mA,
          VectorView<Real, NoConj, Const> tau)
{
    return LAPACKE_dorgqr(LAPACK_COL_MAJOR, mA.nRow(), tau.size(), tau.size(),
                          mA.data(), mA.lDim(), tau.data());
}
}
