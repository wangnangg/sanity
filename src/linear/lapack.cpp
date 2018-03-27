#include "lapack.hpp"
#define HAVE_LAPACK_CONFIG_H
#define LAPACK_COMPLEX_CPP
#include "lapack/lapacke.h"

namespace sanity::linear::lapack
{
static int order = LAPACK_ROW_MAJOR;

int geqrf(MatrixMutableView A, VectorMutableView tau)
{
    return LAPACKE_dgeqrf(order, A.nrow(), A.ncol(), &A(0, 0), A.ldim(),
                          &tau(0));
}
int orgqr(MatrixMutableView mA, VectorConstView tau)
{
    return LAPACKE_dorgqr(order, mA.nrow(), tau.size(), tau.size(), &mA(0, 0),
                          mA.ldim(), &tau(0));
}
}
