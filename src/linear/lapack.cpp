#include "lapack.hpp"
#define HAVE_LAPACK_CONFIG_H
#define LAPACK_COMPLEX_CPP
#include "lapack/lapacke.h"

template <typename M>
static int ml(const M& v)
{
    return v.isRowMajor() ? LAPACK_ROW_MAJOR : LAPACK_COL_MAJOR;
}

namespace sanity::linear::lapack
{
int geqrf(MatrixMutView<Real, General> A, VectorMutView<Real> tau)
{
    return LAPACKE_dgeqrf(ml(A), A.nRow(), A.nCol(), A.data(), A.lDim(),
                          tau.data());
}

int geqrf(MatrixMutView<Complex, General> A, VectorMutView<Complex> tau)
{
    return LAPACKE_zgeqrf(ml(A), A.nRow(), A.nCol(), A.data(), A.lDim(),
                          tau.data());
}

int orgqr(MatrixMutView<Real, General> mA, VectorView<Real> tau)
{
    return LAPACKE_dorgqr(ml(mA), mA.nRow(), tau.size(), tau.size(), mA.data(),
                          mA.lDim(), tau.data());
}
}
