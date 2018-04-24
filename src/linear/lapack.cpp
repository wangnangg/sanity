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

int gesv(MatrixMutableView A, MatrixMutableView B, std::vector<int>& perm)
{
    assert(A.nrow() == A.ncol());
    assert(A.nrow() == B.nrow());
    assert(A.nrow() == (int)perm.size());
    int n = A.nrow();
    return LAPACKE_dgesv(order, n, B.ncol(), &A(0, 0), A.ldim(), &perm[0],
                         &B(0, 0), B.ldim());
}
}  // namespace sanity::linear::lapack
