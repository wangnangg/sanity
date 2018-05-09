#include "lapack.hpp"
#define HAVE_LAPACK_CONFIG_H
#define LAPACK_COMPLEX_CPP
#include "lapack/lapacke.h"

namespace sanity::linear::lapack
{
static int order = LAPACK_ROW_MAJOR;

int geqrf(MatrixMutableView A, VectorMutableView tau)
{
    return LAPACKE_dgeqrf(order, (int)A.nrow(), (int)A.ncol(), &A(0, 0),
                          (int)A.ldim(), &tau(0));
}
int orgqr(MatrixMutableView mA, VectorConstView tau)
{
    return LAPACKE_dorgqr(order, (int)mA.nrow(), (int)tau.size(),
                          (int)tau.size(), &mA(0, 0), (int)mA.ldim(),
                          &tau(0));
}

int gesv(MatrixMutableView A, MatrixMutableView B, std::vector<int>* perm)
{
    assert(A.nrow() == A.ncol());
    assert(A.nrow() == B.nrow());
    int* perm_ptr;
    std::vector<int> perm_vec;
    if (perm)
    {
        assert(A.nrow() == perm->size());
        perm_ptr = &perm->front();
    }
    else
    {
        perm_vec = std::vector<int>(A.nrow());
        perm_ptr = &perm_vec.front();
    }
    uint n = A.nrow();
    return LAPACKE_dgesv(order, (int)n, (int)B.ncol(), &A(0, 0),
                         (int)A.ldim(), perm_ptr, &B(0, 0), (int)B.ldim());
}
}  // namespace sanity::linear::lapack
