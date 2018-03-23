#include "lapack.hpp"
#define HAVE_LAPACK_CONFIG_H
#define LAPACK_COMPLEX_CPP
#include "lapack/lapacke.h"

namespace sanity::linear::lapack
{
static int order = LAPACK_ROW_MAJOR;
int geqrf(MatrixView<Real, General, Mutable> A, VectorView<Real, Mutable> tau)
{
    return LAPACKE_dgeqrf(order, A.nRow(), A.nCol(), A.data(), A.lDim(),
                          tau.data());
}

int orgqr(MatrixView<Real, General, Mutable> mA, VectorView<Real, Const> tau)
{
    return LAPACKE_dorgqr(order, mA.nRow(), tau.size(), tau.size(), mA.data(),
                          mA.lDim(), tau.data());
}
}
